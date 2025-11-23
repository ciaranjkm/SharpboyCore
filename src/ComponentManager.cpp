#include <ComponentManager.h>

//COMPONENT LINKING
bool ComponentManager::link_components(Syncroniser* syncroniser) {
	//LINK BUS TO THE CPU / PPU (dma)
	if (!m_cpu.set_bus_ptr(&m_bus) || !m_ppu.set_bus_ptr(&m_bus)) {
		return false;
	}

	//LINK OTHER COMPS TO THE BUS 
	if (!m_bus.update_imu_ptr(&m_imu) || !m_bus.update_ppu_ptr(&m_ppu) || !m_bus.update_timer_ptr(&m_timer)) {
		return false;
	}

	//LINK SYNCRONISER TO CPU TO ALLOW IT TO TICK THE SYSTEM
	if (!m_cpu.set_sync_ptr(syncroniser)) {
		return false;
	}

	syncroniser->attach_components(this);
	return true;
}

//RESET AND INITIALISE ALL COMPONENTS
void ComponentManager::initialise_components(bool using_boot_rom) {
	m_cpu.reset(using_boot_rom);
	m_imu.reset(using_boot_rom);
	m_ppu.reset(using_boot_rom);
	m_timer.reset(using_boot_rom);
}

bool ComponentManager::assign_cart_type(e_cart_types cart_type) {
	if (m_cartridge) {
		return false;
	}

	m_cartridge.reset();

	switch (cart_type) {
	case CART_ROM:
		m_cartridge = std::make_unique<CartMBC0>();
		m_bus.update_cartridge_ptr(m_cartridge.get());
		return true;

	case CART_ROM_RAM:
		m_cartridge = std::make_unique<CartMBC0>(cart_type, true, false);
		m_bus.update_cartridge_ptr(m_cartridge.get());
		return true;

	case CART_ROM_RAM_BATTERY:
		m_cartridge = std::make_unique<CartMBC0>(cart_type, true, true);
		m_bus.update_cartridge_ptr(m_cartridge.get());
		return true;

	case CART_MBC1:
		m_cartridge = std::make_unique<CartMBC1>(cart_type);
		m_bus.update_cartridge_ptr(m_cartridge.get());
		return true;

	case CART_MBC1_RAM:
		m_cartridge = std::make_unique<CartMBC1>(cart_type);
		m_bus.update_cartridge_ptr(m_cartridge.get());
		return true;

	case CART_MBC1_RAM_BATTERY:
		m_cartridge = std::make_unique<CartMBC1>(cart_type);
		m_bus.update_cartridge_ptr(m_cartridge.get());
		return true;

	default:
		return false;
	}
}

void ComponentManager::load_rom_into_cart(const std::vector<u8>& rom, bool using_boot_rom, const std::vector<u8>& boot_rom) {
	if (using_boot_rom) {
		m_cartridge->load_boot_rom(boot_rom);
	}

	m_cartridge->load_rom(rom);

	if (using_boot_rom) {
		m_cartridge->swap_boot_rom_buffer();
	}
}

//RESET ALL COMPONENTS TO ZERO
void ComponentManager::reset_components() {
	m_cpu.reset();
	m_imu.reset();
	m_ppu.reset();
	m_timer.reset();

	m_cartridge.reset();
	m_cartridge = nullptr;
}

//GETTERS
CPU* ComponentManager::get_cpu() {
	return &m_cpu;
}

Bus* ComponentManager::get_bus() {
	return &m_bus;
}

IMU* ComponentManager::get_imu() {
	return &m_imu;
}

PPU* ComponentManager::get_ppu() {
	return &m_ppu;
}

Timer* ComponentManager::get_timer() {
	return &m_timer;
}

Cartridge* ComponentManager::get_base_cart() {
	return m_cartridge.get();
}