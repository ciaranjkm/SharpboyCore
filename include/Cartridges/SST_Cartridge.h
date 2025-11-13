#include "Cartridge.h"

class CartridgeSST final : public Cartridge {
public:
	CartridgeSST();
	~CartridgeSST();

	bool load_rom(std::string rom_file_name) override;

	u8 read(u16 address) const override;
	void write(u16 address, u8 value) override;

	//sst function
	void unblocked_write(u16 address, u8 value);
	void sst_reset();
};