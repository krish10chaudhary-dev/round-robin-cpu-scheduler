// ============================================================================
//  Round Robin CPU Scheduler using a Circular Queue  (enhanced version)
//
//  Features
//   - Hand-built circular queue (template, wrap-around indexing, auto-grow)
//   - Processes with arrival times (idle CPU time handled correctly)
//   - Configurable time quantum and context-switch cost
//   - Gantt chart, per-process metrics (CT, TAT, WT, RT)
//   - CPU utilisation, throughput, context-switch count
//   - Quantum sweep: shows how quantum size affects performance
//   - Optional trace mode that shows the circular buffer's internals
//
//  Build :  g++ -std=c++17 -O2 -o rr round_robin_scheduler.cpp
//  Run   :  ./rr            (built-in demo)
//           ./rr -i         (enter your own processes)
//           ./rr -t         (trace the circular queue at every dispatch)
// ============================================================================
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

// ---------------------------------------------------------------------------
//  Circular queue: fixed array, head/tail wrap with modulo, doubles when full
// ---------------------------------------------------------------------------
template <typename T>
class CircularQueue {
    T* data;
    int cap, head, tail, count;

    void grow() {                       // unroll the ring into a bigger array
        int ncap = cap * 2;
        T* nd = new T[ncap];
        for (int i = 0; i < count; i++) nd[i] = data[(head + i) % cap];
        delete[] data;
        data = nd; cap = ncap; head = 0; tail = count;
    }

public:
    explicit CircularQueue(int c = 4) : data(new T[c]), cap(c), head(0), tail(0), count(0) {}
    ~CircularQueue() { delete[] data; }
    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;

    void enqueue(const T& v) {
        if (count == cap) grow();
        data[tail] = v;
        tail = (tail + 1) % cap;        // wrap-around
        count++;
    }
    T dequeue() {
        if (empty()) throw underflow_error("dequeue on empty queue");
        T v = data[head];
        head = (head + 1) % cap;        // wrap-around
        count--;
        return v;
    }
    bool empty() const { return count == 0; }
    int  size()  const { return count; }

    // Shows the raw ring, e.g.  [ _ P3 P1 P2 ]  head=2 tail=1
    template <typename F>
    void dump(F name) const {
        cout << "[";
        for (int i = 0; i < cap; i++) {
            bool used = ((i - head + cap) % cap) < count;
            if (used) cout << " " << name(data[i]); else cout << " _";
        }
        cout << " ]  head=" << head << " tail=" << tail << " size=" << count << "\n";
    }
};

// ---------------------------------------------------------------------------
//  Data types
// ---------------------------------------------------------------------------
struct Process {
    int pid, arrival, burst;
    int remaining = 0, firstRun = -1, completion = 0;
};

struct Slice { int pid, start, end; };  // pid: >0 process, -1 idle, -2 context switch

struct Result {
    vector<Process> procs;
    vector<Slice> gantt;
    int switches = 0, total = 0, idle = 0, overhead = 0;
};

// ---------------------------------------------------------------------------
//  Simulation
// ---------------------------------------------------------------------------
Result simulate(vector<Process> P, int quantum, int switchCost, bool trace) {
    Result r;
    int n = P.size();
    for (auto& p : P) { p.remaining = p.burst; p.firstRun = -1; }

    vector<int> order(n);
    for (int i = 0; i < n; i++) order[i] = i;
    sort(order.begin(), order.end(), [&](int a, int b) {
        return P[a].arrival != P[b].arrival ? P[a].arrival < P[b].arrival : P[a].pid < P[b].pid;
    });

    CircularQueue<int> q(4);            // stores indices into P
    int time = 0, next = 0, done = 0, lastPid = -1;

    auto admit = [&]() {                // move everything that has arrived into the queue
        while (next < n && P[order[next]].arrival <= time) q.enqueue(order[next++]);
    };
    auto addSlice = [&](int pid, int s, int e) {
        if (!r.gantt.empty() && r.gantt.back().pid == pid && r.gantt.back().end == s)
            r.gantt.back().end = e;     // merge back-to-back slices of same process
        else r.gantt.push_back({pid, s, e});
    };

    admit();
    while (done < n) {
        if (q.empty()) {                // CPU idle until next arrival
            int t = P[order[next]].arrival;
            addSlice(-1, time, t);
            r.idle += t - time; time = t; lastPid = -1;
            admit();
            continue;
        }
        if (trace) { cout << "t=" << setw(3) << time << "  queue "; q.dump([&](int k) { return "P" + to_string(P[k].pid); }); }

        int i = q.dequeue();
        Process& p = P[i];

        if (lastPid != -1 && lastPid != p.pid) {   // switching to a different process
            r.switches++;
            if (switchCost > 0) {
                addSlice(-2, time, time + switchCost);
                time += switchCost; r.overhead += switchCost;
            }
        }
        lastPid = p.pid;
        if (p.firstRun < 0) p.firstRun = time;

        int run = min(quantum, p.remaining);
        addSlice(p.pid, time, time + run);
        time += run; p.remaining -= run;

        admit();                        // newcomers go in BEFORE the preempted process
        if (p.remaining > 0) q.enqueue(i);
        else { p.completion = time; done++; }
    }
    r.total = time;
    r.procs = P;
    return r;
}

// ---------------------------------------------------------------------------
//  Reporting
// ---------------------------------------------------------------------------
struct Averages { double tat, wt, rt; };

Averages averages(const Result& r) {
    double t = 0, w = 0, rs = 0;
    for (auto& p : r.procs) {
        int tat = p.completion - p.arrival;
        t += tat; w += tat - p.burst; rs += p.firstRun - p.arrival;
    }
    int n = r.procs.size();
    return {t / n, w / n, rs / n};
}

string label(int pid) { return pid == -1 ? "idle" : pid == -2 ? "cs" : "P" + to_string(pid); }

void printGantt(const Result& r) {
    string top = "|", bot;
    for (auto& s : r.gantt) {
        int w = max<int>(6, to_string(s.end).size() + 2);
        string l = label(s.pid);
        int padL = (w - 1 - (int)l.size()) / 2, padR = w - 1 - (int)l.size() - padL;
        top += string(padL, ' ') + l + string(padR, ' ') + "|";
        string st = to_string(s.start);
        bot += st + string(w - st.size(), ' ');
    }
    bot += to_string(r.gantt.back().end);
    cout << "\nGantt chart\n" << top << "\n" << bot << "\n";
}

void printTable(const Result& r) {
    cout << "\n" << left << setw(6) << "PID" << setw(9) << "Arrival" << setw(8) << "Burst"
         << setw(12) << "Completion" << setw(12) << "Turnaround" << setw(9) << "Waiting"
         << "Response\n" << string(65, '-') << "\n";
    for (auto& p : r.procs) {
        int tat = p.completion - p.arrival;
        cout << left << setw(6) << ("P" + to_string(p.pid)) << setw(9) << p.arrival
             << setw(8) << p.burst << setw(12) << p.completion << setw(12) << tat
             << setw(9) << tat - p.burst << p.firstRun - p.arrival << "\n";
    }
    Averages a = averages(r);
    int useful = r.total - r.idle - r.overhead;
    cout << fixed << setprecision(2)
         << "\nAvg turnaround : " << a.tat
         << "\nAvg waiting    : " << a.wt
         << "\nAvg response   : " << a.rt
         << "\nContext switches: " << r.switches
         << "\nCPU utilisation : " << 100.0 * useful / r.total << " %"
         << "\nThroughput      : " << (double)r.procs.size() / r.total << " processes/unit time\n";
}

void printSweep(const vector<Process>& P, int switchCost) {
    int maxBurst = 0;
    for (auto& p : P) maxBurst = max(maxBurst, p.burst);
    cout << "\nQuantum sweep (context-switch cost = " << switchCost << ")\n"
         << left << setw(9) << "Quantum" << setw(11) << "Avg TAT" << setw(11) << "Avg WT"
         << setw(11) << "Avg RT" << setw(10) << "Switches" << "CPU util %\n"
         << string(62, '-') << "\n" << fixed << setprecision(2);
    for (int q = 1; q <= maxBurst; q++) {
        Result r = simulate(P, q, switchCost, false);
        Averages a = averages(r);
        cout << left << setw(9) << q << setw(11) << a.tat << setw(11) << a.wt << setw(11) << a.rt
             << setw(10) << r.switches << 100.0 * (r.total - r.idle - r.overhead) / r.total << "\n";
    }
}

// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------
int main(int argc, char** argv) {
    bool interactive = false, trace = false;
    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        if (a == "-i") interactive = true;
        else if (a == "-t") trace = true;
    }

    vector<Process> procs;
    int quantum = 4, switchCost = 1;

    if (interactive) {
        int n;
        cout << "Number of processes: "; cin >> n;
        for (int i = 1; i <= n; i++) {
            Process p; p.pid = i;
            cout << "P" << i << " arrival burst: "; cin >> p.arrival >> p.burst;
            if (!cin || p.arrival < 0 || p.burst <= 0) { cerr << "Invalid input.\n"; return 1; }
            procs.push_back(p);
        }
        cout << "Time quantum: "; cin >> quantum;
        cout << "Context-switch cost: "; cin >> switchCost;
        if (!cin || quantum <= 0 || switchCost < 0) { cerr << "Invalid input.\n"; return 1; }
    } else {
        procs = {{1, 0, 10}, {2, 1, 4}, {3, 2, 6}, {4, 3, 3}, {5, 40, 5}};  // P5 creates an idle gap
    }

    cout << "Round Robin | quantum = " << quantum << ", context-switch cost = " << switchCost << "\n\n";
    Result r = simulate(procs, quantum, switchCost, trace);
    printGantt(r);
    printTable(r);
    printSweep(procs, switchCost);
    return 0;
}
