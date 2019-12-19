#include <iostream>
#include <fstream>

/**
 * 
 * Simple function that writes the header, so I only have to change it in one place.
 *  
*/
void write_ppm_header(std::ofstream* file)
{
    *(file) << "P3" << '\n';
    *(file) << "#Oskar Mampe" << '\n';
    *(file) << "128 128" << '\n';
    *(file) << "255" << '\n';
}