# If you want to debug the build this is pretty much mandatory on Ubuntu distros

# The reason behind its compulsory application is the need for the parent process
# (gdb usually) to attach to non children processes (the simulator). Ubuntu has this
# permission set to off (probably for safety concerns)
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope