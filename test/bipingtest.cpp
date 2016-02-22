#include <cstdlib>
#include <iostream>
#include "RF24.h"
#include "gpio_sun7i.h"
using namespace std;

const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};
const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 2;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

void setup(RF24& radio)
{
        radio.begin();
        // enable dynamic payloads
        radio.enableDynamicPayloads();
        // optionally, increase the delay between retries & # of retries
        radio.setRetries(15, 15);
        radio.setDataRate(RF24_250KBPS);
        // Open pipes to other nodes for communication
        // Open 'our' pipe for writing
        // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
        radio.openWritingPipe(pipes[0]);
        radio.openReadingPipe(1, pipes[1]);
        // Start listening
        radio.startListening();
        // Dump the configuration of the rf unit for debugging
        radio.printDetails();
}

void loop(RF24& radio)
{
        // Ping out.
        // The payload will always be the same, what will change is how much of it we send.
        static char send_payload[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ789012";

        // First, stop listening so we can talk.
        radio.stopListening();

        // Take the time, and send it.  This will block until complete
        printf("Now sending length %i...", next_payload_size);
        radio.write(send_payload, next_payload_size);

        // Now, continue listening
        radio.startListening();

        // Wait here until we get a response, or timeout
        long started_waiting_at = __millis();

        bool timeout = false;
        while (!radio.available() && !timeout)
                if (__millis() - started_waiting_at > 500) timeout = true;
        // Describe the results
        if (timeout) printf("Failed, response timed out.\n\r");
        else
        {
                // Grab the response, compare, and send to debugging spew
                uint8_t len = radio.getDynamicPayloadSize();
                radio.read(receive_payload, len);
                // Put a zero at the end for easy printing
                receive_payload[len] = 0;
                // Spew it
                printf("Got response size=%i value=%s\n\r", len, receive_payload);
        }
        // Update size for next time.
        next_payload_size += payload_size_increments_by;
        if (next_payload_size > max_payload_size)
                next_payload_size = min_payload_size;

        sleep(1);
}

void PrintUsage( const char* szName )
{
	printf("Usage: %s <-r|-t> <-d device_path>\n", szName );
}

int main(int argc, char** argv)
{
	char* szDeviceName = 0;
	bool bTransmitter = false;

	if (argc < 3)
	{
		printf("Not enough arguments\n");
		PrintUsage( argv[0] );
		return 1;
	}

	char opt = 0;
	while(opt != -1)
	{
		opt = getopt( argc, argv, "rtd:" );
		//wtf 255?!
		if ( -1 == opt || 255 == opt ) break;
		switch(opt)
		{
			case 'd':
				szDeviceName = optarg;
				break;
			case 'r':
				bTransmitter=false;
				break;
			case 't':
				bTransmitter=true;
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
	if ( strstr( szDeviceName, "spidev1" ) )
	{
		CE = UEXT1_CE;
		CSN = UEXT1_CSN;
	}
	else
	{
		CE = UEXT2_CE;
		CSN = UEXT2_CSN;
	}

	// CE - PI14
	// CSN - PI15
	RF24 radio(CE, CSN, szDeviceName);

        setup(radio);
        while(1) loop(radio);

        return 0;
}
