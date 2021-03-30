#https://antc2lt.medium.com/fpga-design-on-macos-big-sur-with-verilog-876ea2ee95b
PROJ = SimpleCounter
TB = counter_tb.v
SRCS = counter.v
all: gtkwave
simulate:
	iverilog -o $(PROJ).vvp $(SRCS) $(TB)
	vvp $(PROJ).vvp > $(PROJ)_logs.txt
gtkwave: simulate
	open io_signals.vcd -a gtkwave
scansion: simulate
	open io_signals.vcd -a Scansion
clean: 
	rm -rf $(PROJ).vvp io_signals.vcd vvp_logs.txt