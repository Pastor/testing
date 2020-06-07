`default_nettype none
`include "baudgen.vh"

module uart(sys_clk, led, tx, rstn, isr);

input wire sys_clk;
output wire [2:0] led;
output wire tx;

input wire rstn;
input wire isr;

wire clk;
wire locked;


PLCC0   PLL_inst(.inclk0( sys_clk ), .c0(clk), .locked(locked));

uart_tx TX0(.clk(clk), .rstn(rstn), .data(data), .start(start), .tx(tx), .ready(ready));

//-- Connecting wires
wire ready;
reg start = 0;
reg [7:0] data;

//-- Multiplexer with the 8-character string to transmit
always @*
  case (char_count)
    8'd00: data <= "0";
    8'd01: data <= "0";
    8'd02: data <= "0";
    8'd03: data <= "0";
    8'd04: data <= "0";
    8'd05: data <= "0";
    8'd06: data <= "0";
    8'd07: data <= "0";
    8'd08: data <= "0";
    8'd09: data <= "0";
    8'd10: data <= "0";
    8'd11: data <= "0";
    8'd12: data <= "0";
    8'd13: data <= "0";
    8'd14: data <= "0";
    8'd15: data <= "0";
    8'd16: data <= "0";
    8'd17: data <= " ";
    8'd18: data <= "0";
    8'd19: data <= "0";
    8'd20: data <= "0";
    8'd21: data <= "0";
    8'd22: data <= "0";
    8'd23: data <= "0";
    8'd24: data <= "0";
    8'd25: data <= "0";
    8'd26: data <= " ";
    8'd27: data <= "R";
    8'd28: data <= " ";
    8'd29: data <= "F";
    8'd30: data <= "F";
    8'd31: data <= "F";
    8'd32: data <= "F";
    8'd33: data <= " ";
    8'd34: data <= "F";
    8'd35: data <= "F";
    8'd36: data <= "\n";
    default: data <= "!";
  endcase

//-- Characters counter
//-- It only counts when the cena control signal is enabled
reg [6:0] char_count;
reg cena;                //-- Counter enable
reg [2:0] switcher = 3'b111;

assign led = switcher;

always @(posedge clk) begin
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
always @(posedge clk) begin
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
	  switcher[0] = ~switcher[0];
	  switcher[1] = ~switcher[1];
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
      if (char_count == 36)
      begin
        next_state = STOP;
        switcher[0] = ~switcher[0];
      end
      else
        next_state = INI;
    end

  endcase
end
	
endmodule