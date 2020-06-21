module Button(input X, output reg Q, input c);
	initial Q <= 1'd1;
	reg [15:0] CTQ;
	reg XQ, RSQ, BQ;
	wire FY = !XQ & BQ;
	always @(posedge c) begin
	  XQ <= !X;
	  if (XQ & ~&CTQ) CTQ <= CTQ + 1'd1;
	  else if (!XQ & !CTQ) CTQ <= CTQ - 1'd1;
	  if (&CTQ) RSQ <= 1'd1;
	  else if (~|CTQ) RSQ <= 0;
	  BQ <= RSQ;
	  if (FY) Q <= !Q;
	end
endmodule