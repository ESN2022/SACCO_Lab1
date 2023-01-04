	library ieee;
	use ieee.std_logic_1164.all;
	use ieee.numeric_std.all;
	
	
	
	entity Ledchaser is
	port (
		clk	: in  std_logic;
      reset	: in  std_logic;
		pio_0 : out std_logic_vector(3 downto 0);
		pio_1 : out std_logic_vector(7 downto 0)
      );
	end entity;
	

		architecture RTL of Ledchaser is
		
	component lab is
		port (
			clk_clk                          : in  std_logic                    := 'X'; -- clk
			reset_reset_n                    : in  std_logic                    := 'X'; -- reset_n
			pio_1_external_connection_export : out std_logic_vector(3 downto 0);        -- export
			pio_0_external_connection_export : out std_logic_vector(7 downto 0)         -- export
		);
	end component lab;
		
		begin
		
	u0 : component lab
		port map (
			clk_clk                          => clk,                          --                       clk.clk
			reset_reset_n                    => reset,                    --                     reset.reset_n
			pio_1_external_connection_export => pio_0, -- pio_1_external_connection.export
			pio_0_external_connection_export => pio_1  -- pio_0_external_connection.export
		);
		
		end architecture;