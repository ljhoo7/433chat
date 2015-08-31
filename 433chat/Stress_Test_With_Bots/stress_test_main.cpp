#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

void main()
{
	/*std::ofstream file("stress.bat");
	for (int m = 0; m < 63; ++m)
	{
		std::string tmpstr = "bot" + std::to_string(m);
		file << "433chat_stress_test_bot 127.0.0.1 9000 " + tmpstr + " 0 1000" << std::endl;
	}
	file.close();*/

	for (int m = 0; m < 63; ++m)
	{
		std::string tmpstr = "bot" + std::to_string(m);
		system(("433chat_stress_test_bot 127.0.0.1 9000 " + tmpstr + " 0 1000").c_str());;
	}
}