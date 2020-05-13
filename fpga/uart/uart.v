`default_nettype none
`include "baudgen.vh"

module uart(sys_clk, led, tx, rstn, isr);

input sys_clk;
output [2:0] led;
output tx;
input rstn;
input isr;

//-- Serial Unit instantation
uart_tx TX0 (
    .clk(sys_clk),
    .rstn(rstn),
    .data(data),
    .start(start),
    .tx(tx),
    .ready(ready)
);

//-- Connecting wires
wire ready;
reg start = 0;
reg [7:0] data;

//-- Multiplexer with the 8-character string to transmit
always @*
  case (char_count)
    8'd0: data <= "H";
    8'd1: data <= "e";
    8'd2: data <= "l";
    8'd3: data <= "l";
    8'd4: data <= "o";
    8'd5: data <= "!";
    8'd6: data <= ".";
    8'd7: data <= ".";
    default: data <= ".";
  endcase

//-- Characters counter
//-- It only counts when the cena control signal is enabled
reg [2:0] char_count;
reg cena;                //-- Counter enable

always @(posedge sys_clk) begin
  if (!rstn)
    char_count = 0;
  else if (cena)
    char_count = char_count + 1;
end

//--------------------- CONTROLLER

localparam INI = 0;
localparam TXCAR = 1;
localparam NEXTCAR = 2;
localparam STOP = 3;

//-- fsm state
reg [1:0] state;
reg [1:0] next_state;

//-- Transition between states
always @(posedge sys_clk) begin
  if (!rstn)
    state <= INI;
  else
    state <= next_state;
end

//-- Control signal generation and next states
always @(*) begin
  next_state = state;
  start = 0;
  cena = 0;

  case (state)
    //-- Initial state. Start the trasmission
    INI: begin
      start = 1;
      next_state = TXCAR;
    end

    //-- Wait until one car is transmitted
    TXCAR: begin
      if (ready)
        next_state = NEXTCAR;
    end

    //-- Increment the character counter
    //-- Finish when it is the last character
    NEXTCAR: begin
      cena = 1;
      if (char_count == 7)
        next_state = STOP;
      else
        next_state = INI;
    end

  endcase
end


//reg	[25:0] count;
//reg [2:0] led;
//
//always @(posedge sys_clk)
//	begin
//		count <= count + 1;
//		if( count == 26'b11_1111_1111_1111_1111_1111_1110 )
//			begin
//			led <= ~led; 
//			end
//		else 
//			led <= led;
//	end
	
endmodule