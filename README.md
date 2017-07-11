# RIPEMD160-30Step
Collision attack on the first 30-step RIPEMD160

In our program, we count the index from 0. However, we count the index from 1 in the paper!
We verify our analysis in this way:
Firstly, we leave 13 bit conditions on Y18 and 1 bit condition on Y22 remaining uncontrolled and we test how many times 
it will run when using the message modification techniques to correct Y0--Y22. We claim that its probability is about 2^{-15}.

Secondly, we generate message words randomly. Then, we generate Y17,Y18,Y19,Y20,Y21 randomly while
at the same time we also have the bit conditions on Y17,Y18,Y19,Y20,Y21 satisfied. In this way, we test how many times it 
will run when the bit conditions on Y23--Y29 hold and Q23--Q29 satisfy their corresponding equations. We claim that its 
probability is about 2^{-23}.

Thirdly, we generate message words randomly and test how many times it will run when the differential path until X26 holds. 
Since there are another four bit conditions on X27 and X28, we estimate that X27 and X28 holds with probability 2^{-4}.
Assume the times the program runs is about 2^{x}, then we can estimate the probability of the left branch as 2^{-4-x}.
We claim that its probability is about 2^{-29}.

After the program is over, three files named "modify.txt", "rightRunningTimes.txt", "leftRunningTimes.txt" will be created.
The details of the results of the program are written in these three files and you can check the result with the above statement.
Of the three files, "modify.txt" corresponds to the verification of message modification, "rightRunningTimes.txt" corresponds
to the uncontrolled part of the left branch (Y23--Y29), and "rightRunningTimes.txt" corresponds to the verification of the left branch.

Please run this program in Visual Studio! You can choose 'Release' instead of 'Debug' so that it will run faster!
