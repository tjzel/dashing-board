set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

idf_component_register(
  SRCS
  "main.cpp"
  "SerialCommunicator.cpp"
  "DigitalSniffer.cpp"
  INCLUDE_DIRS
  "."
  PRIV_REQUIRES
  vfs
  fatfs
  controller)

fatfs_create_spiflash_image(storage data FLASH_IN_PROJECT)
