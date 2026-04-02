Run a few randomly-generated problems with just two jobs and two queues; compute the MLFQ execution trace for each. Make your life easier by limiting the length of each job to 20 and turning off I/Os.

Here's an example setting the random seed to 5—change it to get a different run:

python ./mlfq.py -n 2 -j 2 -m 20 -M 0 -s 5
Choose one of the runs and predict how long jobs will stay in each queue. Note the output tells you how long each queue allotment is.

Then run again with -c at the end of the command line to see if you're right.

How would you run the scheduler to reproduce each of the examples in the chapter (Examples 1-3)?

These are the commands I would use to try to recreate the examples:

# Example 1
python ./mlfq.py -l 0,200,0
# Example 2
python ./mlfq.py -l 0,200,0:100,20,0
# Example 3
python ./mlfq.py -l 0,200,0:0,20,1 -i 10 -S

How would you configure the scheduler parameters to behave just like a round-robin scheduler?

You could make only a single queue, and that way there is nowhere to denote jobs to. 

Craft a workload with two jobs and scheduler parameters so that one job takes advantage of the older Rules 4a and 4b (turned on with the -S flag) to game the scheduler and obtain 99% of the CPU over a particular time interval.

./mlfq.py -n 3 -q 10 -l 0,200,0:0,200,9 -i 1 -S -c

Given a system with a quantum length of 10 ms in its highest queue, how often would you have to boost jobs back to the highest priority level (with the -B flag) in order to guarantee that a single long- running (and potentially-starving) job gets at least 5% of the CPU?

One question that arises in scheduling is which end of a queue to add a job that just finished I/O; the -I flag changes this behavior for this scheduling simulator. Play around with some workloads and see if you can see the effect of this flag.