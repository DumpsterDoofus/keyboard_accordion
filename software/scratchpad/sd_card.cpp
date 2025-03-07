// Taken from https://github.com/PaulStoffregen/SD/blob/Juse_Use_SdFat/examples/ReadWrite/ReadWrite.ino
#include <SD.h>

void printDirectory(File dir, int numTabs)
{
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            break;
        }

        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }

        Serial.print(entry.name());

        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }

        entry.close();
    }
}

void setup()
{
    while (!Serial)
    {
    }

    Serial.print("Initializing SD card...");

    if (!SD.begin(BUILTIN_SDCARD))
    {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

    File myFile = SD.open("test.txt", FILE_WRITE);

    if (myFile)
    {
        Serial.print("Writing to test.txt...");
        myFile.println("testing 1, 2, 3.");
        myFile.close();
        Serial.println("done.");
    }
    else
    {
        Serial.println("error opening test.txt");
    }

    myFile = SD.open("test.txt");
    if (myFile)
    {
        Serial.println("test.txt:");

        while (myFile.available())
        {
            Serial.write(myFile.read());
        }
        myFile.close();
    }
    else
    {
        Serial.println("error opening test.txt");
    }

    auto root = SD.open("/");

    Serial.println("Printing directory tree.");
    printDirectory(root, 0);
    Serial.print("Total space: ");
    Serial.println(SD.totalSize());
    Serial.print("Used space: ");
    Serial.println(SD.usedSize());

    // SD.remove("test.txt");

    Serial.println("Printing directory tree.");
    printDirectory(root, 0);
    Serial.print("Total space: ");
    Serial.println(SD.totalSize());
    Serial.print("Used space: ");
    Serial.println(SD.usedSize());

}

void loop()
{
    if (!SD.begin(BUILTIN_SDCARD))
    {
        Serial.println("initialization failed!");
    }
    else
    {
        Serial.println("initialization succeeded!");
    }

    File myFile = SD.open("test.txt", FILE_READ);

    if (myFile)
    {
        Serial.println("Open file succeeded.");
        Serial.println("File contents:");
        while (myFile.available())
        {
            Serial.write(myFile.read());
        }
    }
    else
    {
        Serial.println("Open file failed.");
    }


    myFile.close();
    delay(1000);
}
