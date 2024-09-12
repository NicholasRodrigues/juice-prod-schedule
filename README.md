# **Juice Production Scheduling Algorithm**

## Nicholas Wrote:
### Hallo, I explained the algorithm to gpt so he can make a summary of it. If u guys dont get it, read the code, I put some comments there, mainly on the algorithm part.
This is only a approach I found to solve the problem, it can be improved in many ways, but I think it is a good start.
I named it "Advanced Greedy Algorithm" because it is a greedy algorithm that tries to balance the penalties and the setup times, and it is dynamic, so it changes the weight of the setup times during the scheduling process.
Kinda bad name, but it was because I think im trying to be too smart, but I'm not, so I will leave it like that.
Its like killing a fly with a bazooka, but it works.

## Nicholas' Explanation for GPT:
#### **Overview**
This project implements an advanced **greedy algorithm** to schedule production tasks for a juice manufacturing line. The goal is to minimize penalties due to late deliveries, while also considering the setup times between switching from one juice type to another. This algorithm incorporates dynamic adjustment of setup time weight during scheduling to balance minimizing penalties and reducing setup overhead.

#### **How the Algorithm Works**
1. **Input Data**:
    - The input consists of multiple juice production orders, where each order has:
        - **Processing time**: The time needed to process the order.
        - **Due time**: The deadline by which the order should be completed.
        - **Penalty rate**: The penalty incurred for each minute the order is delayed past its due time.
        - **Setup times**: The time it takes to switch from producing one type of juice to another.

2. **Initial Setup Time Weight Calculation**:
    - The algorithm calculates an **initial setup time weight** using the averages of penalties, processing times, and setup times. This weight helps determine the importance of reducing setup times compared to minimizing penalties.

3. **Dynamic Setup Time Weight Adjustment**:
    - As the scheduling progresses, the algorithm dynamically adjusts the **setup time weight**:
        - **Early in the schedule**: The algorithm focuses more on minimizing penalties by prioritizing tasks with high penalties and tight deadlines.
        - **Later in the schedule**: The weight for setup times gradually increases, making the algorithm focus more on minimizing unnecessary task transitions to finish the schedule efficiently.

4. **Priority Calculation**:
    - The priority for each task is calculated using the following formula:
      
       `Priority = (Penalty Rate / Time Left) * (1 / Processing Time) + Dynamic Setup Time Weight * Setup Time`
   
      - The task with the highest priority is selected for scheduling next.

5. **Dynamic Behavior**:
    - At each step, the **dynamic setup time weight** is adjusted based on the number of tasks already scheduled. The algorithm balances between minimizing penalties and reducing setup overhead dynamically throughout the scheduling process.

#### **How to Run the Algorithm**

1. **Build the Project**:
   To build the project, use the following commands:

   ```bash
   cd build
   cmake ..
   make
   ```

2. **Run the Program**:
   The program will read input files from the `data/` directory and process each file using the advanced greedy algorithm. You can run it using:

   ```bash
   ./main
   ```

   The output will display the order of tasks scheduled for each input file, showing how the algorithm balances penalties and setup times dynamically.

#### **Input File Format**
Each input file follows this format:
```
<number_of_orders>
<processing_times>
<due_times>
<penalty_rates>
<setup_time_matrix>
```
Example (`input1.txt`):
```
5
15 25 20 30 20
25 45 75 120 135
10 12 30 15 10
0 10 15 8 21
10 0 10 13 9
17 9 0 10 14
11 13 12 0 10
5 10 15 20 0
```

#### **Future Improvements**
- **Metaheuristics**: This greedy algorithm can serve as the base for more advanced metaheuristics such as **GRASP**, **VND**, or **ILS**.
- **Parameter Tuning**: The current setup time weight is dynamically adjusted but could be fine-tuned further based on specific production needs.
