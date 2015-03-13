#include <cstdint>
#include <sstream>
#include <control/libclient/ControlClient.hpp>

class scv3_rf {

  CbmNet::ControlClient ctrlclient;  
  static const uint32_t cnet_nodeid = 0x8000; // cbmnet addr of auxblock
  static const uint32_t cnet_rf_offset = 0x2f0000; // offset for cbmnet moduel rf 
  static const size_t number_of_fe_links = 6;
  static const size_t number_of_lanes = 4;

public:

  scv3_rf(std::string path) 
    : ctrlclient()
  {
    ctrlclient.Connect(path);  
  }
  
  ~scv3_rf() {}
  
  int read_cnet(uint32_t addr, uint32_t& value) {
    return ctrlclient.Read(cnet_nodeid, addr + cnet_rf_offset, value);
  }
  
  
  int write_cnet(uint32_t addr, uint32_t value) {
    return ctrlclient.Write(cnet_nodeid, addr + cnet_rf_offset, value);
  }
  
  void reset_sc(){
    ctrlclient.SendDLM(0, 15);
    ctrlclient.SendDLM(0, 15);
  }
    

  int read_per_lane_cntr(uint32_t base_addr, size_t fe_link, 
			 size_t lane, uint32_t& value) {
    assert(fe_link < number_of_fe_links);
    assert(lane < number_of_lanes);
    
    uint32_t addr = base_addr + 
      sizeof(uint64_t)*number_of_lanes*fe_link + 
      sizeof(uint64_t)*lane;
    return read_cnet(addr, value);
  }
  

  int read_per_link_cntr(uint32_t base_addr, size_t fe_link, 
			 uint32_t& value) {
    assert(fe_link < number_of_fe_links);

    uint32_t addr = base_addr + 
      sizeof(uint64_t)*number_of_lanes*fe_link;
    std::cout << std::hex << addr << std::endl;
    return read_cnet(addr, value);
  }

  
  std::string print_per_lane_cntr(uint32_t base_addr, size_t fe_link, 
				  size_t lane, uint32_t& value) {
    std::stringstream ss;
    if (read_per_lane_cntr(base_addr, fe_link, lane, value) > 0) {
      ss << value;
    } else {
      ss << "read failed";	
    }
    return ss.str();
  }


  std::string print_per_link_cntr(uint32_t base_addr, size_t fe_link, 
				  uint32_t& value) {
    std::stringstream ss;
    if (read_per_link_cntr(base_addr, fe_link, value) > 0) {
      ss << value;
    } else {
      ss << "read failed";	
    }
    return ss.str();
  }


};
