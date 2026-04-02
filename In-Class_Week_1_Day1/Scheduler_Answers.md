Compute the response time and turnaround time when running three jobs of length 200 with the SJF and FIFO schedulers.

python scheduler.py -l 200,200,200 -p SJF
Try to predict the times, then run again adding the -c switch to check your answer.

response: 0, 200, 400
turnaround: 200, 400, 600

Now do the same but with jobs of different lengths: 100, 200, and 300.

response: 0, 100, 300
turnaround: 100, 300, 600

Now do the same, but also with the RR scheduler and a time-slice of 1.

response: 0, 1, 2
turnaround: 298, 499, 600

For what types of workloads does SJF deliver the same turnaround times as FIFO?

FIFO and SJF give the same turnaround times when the input is already in shortest-to-longest order or when all jobs are the same length.

For what types of workloads and quantum lengths does SJF deliver the same response times as RR?

The responses are the same when the jobs are already in SJF order, the quantum is large enough to run each job to completion.

What happens to response time with SJF as job lengths increase? Can you use the simulator to demonstrate the trend?

The response time for any job with this organization is the sum of all jobs smaller than it, so in general as job sizes grown, the response times for later jobs grow as well.

What happens to response time with RR as quantum lengths increase? Can you write an equation that gives the worst-case response time, given N jobs? Hint: run the simulation a number of times with jobs of the same length and a variety of quanta and see if you can find the pattern. (Or consider: where does a job have to be to have to wait the longest?)

If the quantum for RR is equal to or larger than the jobs, it performs no better than FIFO.