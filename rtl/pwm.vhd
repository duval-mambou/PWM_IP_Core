-- *****************************************************
-- **  PWM                                            **
-- **   32 bits device for Altera/Avalon interface     **
-- *****************************************************

-- *****************************************************
-- **  Register mapping                               **
-- *****************************************************
-- *   Register      |  Reg Num  |  Access  |  Reset   *
-- ******************|***********|**********|***********
-- *   Status        |    0      |  RO      |  0       *
-- *   SetCtrl       |    0      |  WO      |see note 1*
-- *   Period        |    1      |  R/W     |0xffffffff*
-- *   DCycle        |    2      |  R/W     |  0       *
-- *   NbCycles      |    3      |  R/W     |0xffff    *
-- *   ClrCtrl       |    4      |  WO      |see note 1*
-- *****************************************************
-- note 1: internal Ctrl register is initialized at 0

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity pwm is
	port(
		Clk, Reset_n, WritePWM, ReadPWM : in std_logic;
		OutPWM : out std_logic;
		RegsAddr : in std_logic_vector(2 downto 0);
		DInPWM : in std_logic_vector(31 downto 0);
		DOutPWM : out std_logic_vector(31 downto 0);
		IrqPWM : out std_logic
		);
end entity pwm;


architecture rtl of pwm is
signal SynRst_n0, SynRst_n1 : std_logic;
signal ldDC, ldP, SetCtrl, ClrCtrl, ldNbCyc : std_logic;
signal IrqEn, Start : std_logic;
signal razIrq, setIrq, IrqRq, PulseState, PWMState : std_logic;
signal EndPulse, Pulse, EndCycles : std_logic;
signal DCycle, Period, DCnt : std_logic_vector(31 downto 0);
signal NbCyc, CntCyc : std_logic_vector(15 downto 0);

constant Zero8 : std_logic_vector(7 downto 0) := "00000000";

begin

-- Reset synchronization
Rst : process(Reset_n, Clk)
begin
	if(Reset_n = '0') then
		SynRst_n0 <= '0';
		SynRst_n1 <= '0';
	elsif(rising_edge(Clk)) then
		SynRst_n0 <= '1';
		SynRst_n1 <= SynRst_n0;
	end if;
end process Rst;

-- Status Register
Status : process(SynRst_n1, Clk)
begin
	if(SynRst_n1 = '0') then       --asynchronous reset
		PWMState <= '0';
		PulseState <= '0';
		IrqRq <= '0';
	elsif(rising_edge(Clk)) then
		PWMState <= Start;
		PulseState <= Pulse;
		if(razIrq = '1') then
			IrqRq <= '0';
		elsif(setIrq = '1') then
			IrqRq <= '1';
		end if;
	end if;
end process Status;

-- Set & Clear Control Register
Ctrl : process(SynRst_n1, Clk)
begin
    if(SynRst_n1 = '0') then        --asynchronous reset
        Start <= '0';
        IrqEn <= '0';
    elsif(rising_edge(Clk)) then
        if(ClrCtrl = '1') then
            if(DInPWM(0) = '1') then
                Start <= '0';
            end if;
            if(DInPWM(1) = '1') then
                IrqEn <= '0';
            end if;
        elsif(SetCtrl = '1') then
            if(DInPWM(0) = '1') then
                Start <= '1';
            end if;
            if(DInPWM(1) = '1') then
                IrqEn <= '1';
            end if;
        end if;
	end if;
end process Ctrl;

-- Period of the PWM signal
PeriodReg : process(SynRst_n1, Clk)
begin
	if(SynRst_n1 = '0') then       --asynchronous reset
		Period <= (others => '1');
	elsif(rising_edge(Clk)) then
		if(ldP = '1') then
			Period <= DInPWM;
		end if;
	end if;
end process PeriodReg;

-- Duty cycle of the PWM signal
DCycleReg : process(SynRst_n1, Clk)
begin
	if(SynRst_n1 = '0') then       --asynchronous reset
		DCycle <= (others => '0');
	elsif(rising_edge(Clk)) then
		if (ldP = '1') then
			DCycle <= (others => '0');
		elsif(ldDC = '1') then
			DCycle <= DInPWM;
		end if;
	end if;
end process DCycleReg;

-- Internal countdown
DCntReg : process(Clk)
begin
	if(rising_edge(Clk)) then
		if((EndPulse = '1') or (ldP = '1') or (ldDC = '1') or (Start = '0')) then
			DCnt <= std_logic_vector(unsigned(Period) - 1);
		else
			DCnt <= std_logic_vector(unsigned(DCnt) - 1);
		end if;
	end if;
end process DCntReg;

-- Number of cycles
NbCycReg : process(SynRst_n1, Clk)
begin
	if(SynRst_n1 = '0') then       --asynchronous reset
		NbCyc <= (others => '1');
	elsif(rising_edge(Clk)) then
		if(ldNbCyc = '1') then
			NbCyc <= DInPWM(15 downto 0);
		end if;
	end if;
end process NbCycReg;

-- Cycles counter
CntCycReg : process(SynRst_n1, Clk)
begin
	if(SynRst_n1 = '0') then
		CntCyc <= (others => '0');
	elsif(rising_edge(Clk)) then
		if((EndCycles = '1') or (ldNbCyc = '1') or (ldP = '1') or (ldDC = '1') or (Start = '0')) then
			CntCyc <= (others => '0');
		elsif(EndPulse = '1') then
			CntCyc <= std_logic_vector(unsigned(CntCyc) + 1);
		end if;
	end if;
end process CntCycReg;

setIrq <= EndCycles;
OutPWM <= PulseState;
IrqPWM <= IrqRq and IrqEn;

-- Reached duty cycle comparator
Pulse <=	'1' when (unsigned(DCnt) < unsigned(DCycle)) else
			'0';

-- Elapsed period comparator					
EndPulse <=	'1' when (unsigned(DCnt) = 0) else
			'0';
			
EndCycles <= '1' when (unsigned(CntCyc) = unsigned(NbCyc)) else
			 '0';
					
-- Output bus
DOutPWM <=	Period	when (RegsAddr = "001") else					-- offset 1
			DCycle	when (RegsAddr = "010") else					-- offset 2
			zero8&zero8&NbCyc when (RegsAddr = "011") else			-- offset 3
            zero8&zero8&zero8&"00000"&IrqRq&PulseState&PWMState;	-- Default register				

-- Decoder
razIrq <=	'1' when (ReadPWM = '1' and RegsAddr = "000") else
			'0';
            
SetCtrl <=  '1' when (WritePWM = '1' and RegsAddr = "000") else
			'0';

ldP <=		'1' when (WritePWM = '1' and RegsAddr = "001") else
		    '0';

ldDC <=		'1' when (WritePWM = '1' and RegsAddr = "010") else
			'0';

ldNbCyc <=	'1' when (WritePWM = '1' and RegsAddr = "011") else
			'0';
            
ClrCtrl <=  '1' when (WritePWM = '1' and RegsAddr = "100") else
			'0';

end architecture rtl;