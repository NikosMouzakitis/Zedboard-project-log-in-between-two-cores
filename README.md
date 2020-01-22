# Zedboard-project-log-in-between-two-cores

Core A9_0 asks user for a password.
Core A9_1 accepts or not the password provided by the A9_0 core.
Communication occurs throught shared memory.
Note that in liker script is essential to change the base address for the one core's code so they
can run simultaneously.
