#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#ifdef GPIO_SUN7I
#include "gpio_sun7i.h"
#else
#include "gpio_sun4i.h"
#endif
// // Hardware configuration // // Set up nRF24L01 radio on
//SPI bus plus pins 8 & 9
void setup(RF24& radio);
void loop(RF24& radio);


//RF24 radio(SUNXI_GPI(14), SUNXI_GPI(15), "/dev/spidev2.0"); // // Channel info 
//
const short num_channels = 128;
short values[num_channels]; //

void PrintUsage( const char* szName )
{
	printf("Usage: %s <-d device_path>\n", szName );
}

int main(int argc, char** argv)
{
	char* szDeviceName = 0;

	if (argc < 2)
	{
		printf("Not enough arguments\n");
		PrintUsage( argv[0] );
		return 1;
	}

	char opt = 0;
	while(opt != -1)
	{
		opt = getopt( argc, argv, "d:" );
		//wtf 255?!
		if ( -1 == opt || 255 == opt ) break;
		switch(opt)
		{
			case 'd':
				szDeviceName = optarg;
				break;
			case '?':
			default:
				printf("Unknown adgument %c[%d]\n", opt,opt );
				PrintUsage( argv[0] );
				return 1;
				break;
		}
	}

	int CE = 0, CSN = 0;

	//lame but works for now
	if ( strstr( szDeviceName, "spidev2" ) )
	{
		CE = UEXT1_CE;
		CSN = UEXT1_CSN;
	}
	else
	{
		CE = UEXT2_CE;
		CSN = UEXT2_CSN;
	}

	RF24 radio(CE, CSN, szDeviceName);

  setup( radio );
  while (1) loop( radio );

  return 0;
}

// Setup //
void setup(RF24& radio) {
  //
  // Print preamble
  //

  printf("\n\rRF24/examples/scanner/\n\r");
  //
  // Setup and configure rf radio
  //
  radio.begin();
  radio.setAutoAck(false);
  radio.disableCRC();
  radio.setRetries( 0, 0 );
  // Get into standby mode
  radio.startListening();
  radio.stopListening();
  // Print out header, high then low digit
  radio.printDetails();
  int i = 0;
  while ( i < num_channels )
  {
    printf("%x",i>>4);
    ++i;
  }
  printf("\n\r");
  i = 0;
  while ( i < num_channels )
  {
    printf("%x",i&0xf);
    ++i;
  }
  printf("\n\r");
}
// // Loop //
const short num_reps = 100;

void loop(RF24& radio) {
  // Clear measurement values
  memset(values,0,num_channels);
  // Scan all channels num_reps times
  int rep_counter = num_reps;
  while (rep_counter--)
  {
    int i = num_channels;
    while (i--)
    {
      // Select this channel
      radio.setChannel(i);
      // Listen for a little
      radio.startListening();
      __usleep(128);
      radio.stopListening();
      // Did we get a carrier?
      if ( radio.testCarrier() )
	++values[i];
    }
  }
  // Print out channel measurements, clamped to a single hex digit
  int i = 0;
  while ( i < num_channels )
  {
    printf("%x",min(0xf,values[i]&0xf));
    ++i;
  }
  printf("\n\r");
}
