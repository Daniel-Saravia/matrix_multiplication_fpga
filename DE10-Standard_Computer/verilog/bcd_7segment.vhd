library ieee;
use ieee.std_logic_1164.all;

entity bcd_7segment is
    port (
        bcd_in : in std_logic_vector (3 downto 0);
        seven_segment_out : out std_logic_vector (6 downto 0)
    );
end bcd_7segment;

architecture Behavioral of bcd_7segment is
begin
    process (bcd_in)
        variable A : STD_LOGIC := bcd_in(3);
        variable B : STD_LOGIC := bcd_in(2);
        variable C : STD_LOGIC := bcd_in(1);
        variable D : STD_LOGIC := bcd_in(0);
    begin
        seven_segment_out(0) <= not ((not B and not D) or C or (B and D) or A);
        seven_segment_out(1) <= not (not B or (not C and not D) or (C and D));
        seven_segment_out(2) <= not (not C or D or B);
        seven_segment_out(3) <= not ((not B and not D) or (not B and C) or (B and not C and D) or (C and not D) or A);
        seven_segment_out(4) <= not ((not B and not D) or (C and not D));
        seven_segment_out(5) <= not ((not C and not D) or (B and not C) or (B and not D) or A);
        seven_segment_out(6) <= not ((not B and C) or (B and not C) or A or (B and not D));
    end process;
end Behavioral;
