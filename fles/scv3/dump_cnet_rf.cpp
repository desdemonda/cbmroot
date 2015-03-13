#include <cstdint>
#include "cbmnet_registers.h"
#include "scv3_rf.cpp"


int main(int argc, const char* argv[])
{

  std::stringstream path;
  size_t link = 1;
  path << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + link;
  
  scv3_rf rf(path.str());

  uint32_t value = 0;
  std::cout << "rx_eye_width" << std::endl;
  for (size_t fe_link = 0; fe_link <= 5; fe_link++) {
    std::cout << "  fe_link " << fe_link << std::endl;
    for (size_t lane = 0; lane <= 3; lane++) {
	std::cout << "    lane " << lane << ": " 
		  <<  rf.print_per_lane_cntr(RX_EYE, fe_link, lane, value) 
		  << std::endl;
    }
  } 
   
  return 0;
}
