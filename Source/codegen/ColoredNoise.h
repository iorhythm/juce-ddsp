//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  ColouredNoiseA.h
//
//  Code generation for function 'ColouredNoiseA'
//
#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <array>

#include "fft.h"
#include "ifft.h"
#include "../DDSPCommon.h"


namespace coder::dsp
{
	void genrand_uint32_vector( std::array< unsigned, 625 >&, unsigned u[ 2 ] );
	void randn( DDSP::Amplitudes&, std::array< unsigned, 625 >& );

	//________________________________________________________________________________
	class ColouredNoiseA
	{
	public:
		ColouredNoiseA* init();
		void step( DDSP::Amplitudes&, std::array< unsigned, 625 >& );
		void setupAndReset();
		void setup();
		void release();
	private:
		int isInitialized;
		boolean_T isSetupComplete;
	};

	//________________________________________________________________________________
	class ColouredNoiseB
	{
	public:
		static constexpr double num[ 30 ]
		{
			1.0, 1.0, 1.0, 1.0, 1.0, 0.768234948541554,
			0.864504936506425, -1.961428522477521, 0.060383703505529,
			-0.227172523705096, 0.16, 0.003466282314942, 0.961476155209161, 0.037249,
			0.017161, 1.0, 1.0, 1.0, 1.0, 1.0, 0.576176211406166, -0.102709775958803,
			-1.9982397287631271, 0.036918533749494, -0.176882423037556, 0.09,
			-0.82789728252825, 0.998240489034364, 0.013924, 0.010404
		};

		ColouredNoiseB* init();
		void step( DDSP::Amplitudes&, std::array< unsigned, 625 >& );
		void setupAndReset();
		void setup();
		void release();
	protected:
		double pFilterStates[ 10 ];
	private:
		int isInitialized;
		boolean_T isSetupComplete;
	};

	//________________________________________________________________________________
	class ColouredNoiseC
	{
	public:
		static constexpr std::array< double, 256 > dv1
		{ 
		1.0, -0.99500000000000011,
			-0.0024874999999999472, -0.00083331249999998218, -0.00041769789062499104,
			-0.00025103643226561962, -0.00016756681853730108,
			-0.00011981027525417027, -8.9932587862661564E-5, -6.9997530886438259E-5,
			-5.6033023474593834E-5, -4.58706705807925E-5, -3.8244671596735745E-5,
			-3.2375585455544376E-5, -2.7762064528129306E-5, -2.4069709945888108E-5,
			-2.1068517987010183E-5, -1.8596065435005162E-5, -1.6535001515958756E-5,
			-1.4798826356783085E-5, -1.3322643427693973E-5, -1.2056992302063045E-5,
			-1.0963642318307781E-5, -1.0012665517219781E-5, -9.1803626961008854E-6,
			-8.4477697529520348E-6, -7.7995658815236E-6, -7.2232646247221341E-6,
			-6.7086070202106826E-6, -6.2471011234754995E-6, -5.8316688987643787E-6,
			-5.4563727873761542E-6, -5.1162020464131728E-6, -4.8069043772436488E-6,
			-4.5248521939318533E-6, -4.2669356188777379E-6, -4.030476269998263E-6,
			-3.8131573467916E-6, -3.6129665860850414E-6, -3.4281494491814603E-6,
			-3.2571704954035351E-6, -3.0986813456881677E-6, -2.95149398176798E-6,
			-2.8145583888929306E-6, -2.6869437528510809E-6, -2.5678225798080166E-6,
			-2.4564572309663432E-6, -2.3521884612689424E-6, -2.2544256283474518E-6,
			-2.162638299193306E-6, -2.0763490310554934E-6, -1.9951271424877344E-6,
			-1.9185833223095994E-6, -1.8463649500830403E-6, -1.778152022760528E-6,
			-1.7136535993894871E-6, -1.6526046899112366E-6, -1.5947635257643434E-6,
			-1.5399091596626217E-6, -1.4878393499418264E-6, -1.4383686915562608E-6,
			-1.3913269613979863E-6, -1.3465576503013898E-6, -1.3039166580418457E-6,
			-1.2632711309669475E-6, -1.2244984247165004E-6, -1.1874851768784789E-6,
			-1.1521264764624705E-6, -1.1183251188074318E-6, -1.0859909360245212E-6,
			-1.0550401943478222E-6, -1.0253950508587529E-6, -9.9698306299120824E-7,
			-9.6973674503685927E-7, -9.4359316657268971E-7, -9.18493588341856E-7,
			-8.9438313164788223E-7, -8.7121047778245972E-7, -8.4892759440840824E-7,
			-8.2748948616986684E-7, -8.0685396710850571E-7, -7.8698145273342592E-7,
			-7.67834769828509E-7, -7.4937898228865492E-7, -7.3158123145929928E-7,
			-7.144105896150487E-7, -6.9783792535595538E-7, -6.8183577982624121E-7,
			-6.6637825277222583E-7, -6.5144089755558991E-7, -6.37000624326441E-7,
			-6.2303561063928436E-7, -6.0952521886509545E-7, -5.9644991981524734E-7,
			-5.8379122204895559E-7, -5.7153160638592752E-7, -5.5965446519072E-7,
			-5.4814404603550868E-7, -5.369853993840715E-7, -5.26164329972241E-7,
			-5.1566735158929473E-7, -5.0548164499107046E-7, -4.9559501869933327E-7,
			-4.8599587246336074E-7, -4.7667316317908757E-7, -4.6761637307868494E-7,
			-4.5881548001932664E-7, -4.502609297142934E-7, -4.4194360976262655E-7,
			-4.3385482534541148E-7, -4.25986276467556E-7, -4.1833003663374723E-7,
			-4.108785328562086E-7, -4.0362452690003039E-7, -3.9656109767927983E-7,
			-3.8968162472388711E-7, -3.8297977264350641E-7, -3.7644947652022611E-7,
			-3.7008492816719348E-7, -3.6388056319497876E-7, -3.5783104883186218E-7,
			-3.5193127244822936E-7, -3.4617633073893245E-7, -3.4056151952084975E-7,
			-3.3508232410597803E-7, -3.2973441021324662E-7, -3.2451361538487019E-7,
			-3.1941594087547793E-7, -3.14437543984489E-7, -3.0957473080426376E-7,
			-3.0482394935845982E-7, -3.0018178310677945E-7, -2.9564494479391562E-7,
			-2.9121027062200689E-7, -2.8687471472729862E-7, -2.8263534394299521E-7,
			-2.7848933283147847E-7, -2.7443395897017334E-7, -2.7046659847636541E-7,
			-2.6658472175722619E-7, -2.6278588947218572E-7, -2.5906774869561118E-7,
			-2.5542802926851437E-7, -2.5186454032871934E-7, -2.4837516700958184E-7,
			-2.4495786729796722E-7, -2.4161066904276558E-7, -2.3833166710575662E-7,
			-2.351190206471351E-7, -2.3197095053847045E-7, -2.2888573689630877E-7,
			-2.2586171673002974E-7, -2.2289728169794808E-7, -2.1999087596600425E-7,
			-2.1714099416371736E-7, -2.1434617943238435E-7, -2.1160502156079713E-7,
			-2.0891615520402141E-7, -2.0627825818103392E-7, -2.03690049847253E-7,
			-2.0115028953822005E-7, -1.9865777508089859E-7, -1.9621134136925417E-7,
			-1.9380985900096178E-7, -1.9145223297226103E-7, -1.8913740142814189E-7,
			-1.8686433446519522E-7, -1.84632032984608E-7, -1.824395275929158E-7,
			-1.8028587754825386E-7, -1.78170169749967E-7, -1.7609151776955072E-7,
			-1.7404906092100272E-7, -1.7204196336876341E-7, -1.7006941327151811E-7,
			-1.6813062196022279E-7, -1.6622482314879866E-7, -1.6435127217601982E-7,
			-1.6250924527719307E-7, -1.606980388842992E-7, -1.5891696895333154E-7,
			-1.5716537031763046E-7, -1.5544259606607181E-7, -1.5374801694502366E-7,
			-1.5208102078303822E-7, -1.5044101193729681E-7, -1.4882741076087256E-7,
			-1.47239653089921E-7, -1.4567718974995083E-7, -1.4413948608036803E-7,
			-1.4262602147652416E-7, -1.4113628894853639E-7, -1.3966979469618051E-7,
			-1.3822605769919148E-7, -1.3680460932233637E-7, -1.3540499293465402E-7,
			-1.3402676354228344E-7, -1.3266948743432986E-7, -1.3133274184124153E-7,
			-1.3001611460519491E-7, -1.2871920386200809E-7, -1.2744161773412398E-7,
			-1.2618297403422508E-7, -1.2494289997906112E-7, -1.2372103191308943E-7,
			-1.2251701504154497E-7, -1.2133050317257466E-7, -1.2016115846808537E-7,
			-1.1900865120297079E-7, -1.1787265953239698E-7, -1.167528692668392E-7,
			-1.156489736545769E-7, -1.145606731713652E-7, -1.1348767531701369E-7,
			-1.1242969441861443E-7, -1.1138645144017192E-7, -1.1035767379839811E-7,
			-1.0934309518444508E-7, -1.0834245539135807E-7, -1.0735550014703954E-7,
			-1.0638198095252434E-7, -1.0542165492537372E-7, -1.044742846480038E-7,
			-1.0353963802077167E-7, -1.0261748811964916E-7, -1.0170761305832159E-7,
			-1.0080979585455455E-7, -9.9923824300678616E-8, -9.9049490838047681E-8,
			-9.8186592435331941E-8, -9.7334930470512431E-8, -9.6494310616448917E-8,
			-9.5664542729897988E-8, -9.4845440743863019E-8, -9.4036822563162137E-8,
			-9.3238509963104662E-8, -9.2450328491170791E-8, -9.16721073715932E-8,
			-9.0903679412743093E-8, -9.0144880917226679E-8, -8.9395551594602241E-8,
			-8.8655534476630323E-8, -8.79246758349738E-8, -8.7202825101266909E-8,
			-8.648983478947581E-8, -8.57855604204758E-8, -8.5089860448773168E-8,
			-8.4402596191302307E-8, -8.3723631758231152E-8, -8.3052833985710385E-8,
			-8.2390072370504409E-8, -8.1735219006444025E-8, -8.1088148522643E-8,
			-8.044873802342296E-8, -7.9816867029892521E-8, -7.9192417423129237E-8 
		};

		ColouredNoiseC* init();
		void step( DDSP::Amplitudes&, std::array< unsigned, 625 >& );
		void setupAndReset();
		void setup();
		void release();
	protected:
		double pFilterStates[ 255 ];
	private:
		std::array< double, 4096> out;
		std::array< double, 255> dv;

		int isInitialized;
		boolean_T isSetupComplete;
	};
}
