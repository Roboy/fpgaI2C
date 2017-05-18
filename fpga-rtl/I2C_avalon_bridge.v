// I2C_avalon_bridge node
// you can read out the registers via avalon bus in the following way:
// #define IORD(base,reg) (*(((volatile uint32_t*)base)+reg))
// #define IOWR(base,reg,data) (*(((volatile uint32_t*)base)+reg)=data)
// where reg corresponds to the address of the avalon slave

`timescale 1ns/10ps

module I2C_avalon_bridge (
	input clock,
	input reset,
	// this is for the avalon interface
	input [2:0] address,
	input write,
	input signed [31:0] writedata,
	input read,
	output signed [31:0] readdata,
	output waitrequest,
//	output interrupt_sender_irq,
	// these are the i2c ports
	inout scl,
	inout sda
);

reg [6:0] addr;
reg [7:0] data_rd;
reg [7:0] data_wr;
reg rw;
reg busy;
reg ack_error;
reg ena;
reg [2:0] number_of_bytes;
wire [2:0] byte_counter;
reg busy_prev;
reg [31:0] data_read;
reg [31:0] data_write;

assign readdata = 
	((address == 0))? addr :
	((address == 1))? data_read :
	((address == 2))? rw :
	((address == 3))? ena :
	((address == 4))? busy :
	((address == 5))? ack_error :
	32'hDEAD_BEEF;
	
always @(posedge clock, posedge reset) begin: I2C_CONTROL_LOGIC
	if (reset == 1) begin 
		data_read <= 0;
		data_write <= 0;
		ena <= 0;
	end else begin
		// if we are writing via avalon bus and waitrequest is deasserted, write the respective register
		if(write && ~waitrequest) begin
			case(address)
				0: addr <= writedata; 
				1: data_wr <= writedata; 
				2: rw <= writedata; 
				3: ena <= writedata;
				4: number_of_bytes <= writedata;
			endcase 
		end
		if(byte_counter>=number_of_bytes) 
			ena <= 0;
		if( busy==0 ) begin
			if (byte_counter == 0) begin
				data_read[7:0] <= data_rd;
				data_wr <= data_write[7:0];
			end else if(byte_counter == 1) begin
				data_read[15:0] <= data_rd;
				data_wr <= data_write[15:0];
			end else if(byte_counter == 2) begin
				data_read[23:16] <= data_rd;
				data_wr <= data_write[23:16];
			end else if(byte_counter == 3) begin
				data_read[31:24] <= data_rd;
				data_wr <= data_write[31:24];
			end
		end
	end 
end

// if i2c node is busy we have to wait
assign waitrequest = busy||ena ;

i2c_master i2c(
	.clk(clock),
	.reset_n(~reset),
	.ena(ena),
	.addr(addr),
	.rw(rw),
	.data_wr(data_wr),
	.busy(busy),
	.data_rd(data_rd),
	.ack_error(ack_error),
	.sda(sda),
	.scl(scl),
	.byte_counter(byte_counter)
);

endmodule

