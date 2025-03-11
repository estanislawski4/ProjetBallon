#include "telemetrie.h"
int main(int argc, char **argv)
{
    try{
        Telemetrie telemetrie;
        string trame = telemetrie.CreationTrameAPRS();
        std::cout << trame << endl;
    }

    catch (const std::exception &e)
    {
        std::cerr << "Exception attrapée: " << e.what() << std::endl;
    }
}
