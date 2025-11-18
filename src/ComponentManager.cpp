#include <ComponentManager.h>

//COMPONENT LINKING
void ComponentManager::link_components() {
	//link bus to cpu
	m_cpu.set_bus_ptr(&m_bus);

	m_components_linked = false;

	//link components to the bus
	if (!m_bus.update_imu_ptr(&m_imu) || !m_bus.update_ppu_ptr(&m_ppu) || !m_bus.update_timer_ptr(&m_timer)) {
		return;
	}

	m_components_linked = true;
}

bool ComponentManager::is_linked() const {
	return m_components_linked;
}

void ComponentManager::add_syncroniser(Syncroniser* m_sync) {
	m_cpu.set_sync_ptr(m_sync);
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
		Logger::log(log_error, "Already a cartridge object");
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

	default:
		return false;
	}
}

void ComponentManager::load_rom_into_cart(const std::vector<u8>& rom, bool using_boot_rom, const std::vector<u8>& boot_rom) {
	if (using_boot_rom) {
		m_cartridge->load_boot_rom(boot_rom);
	}

	m_cartridge->load_rom(rom);
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