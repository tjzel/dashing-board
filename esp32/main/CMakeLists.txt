set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

idf_component_register(
  SRCS
  "main.cpp"
  "SerialCommunicator.cpp"
  "BLEDataServer.cpp"
  INCLUDE_DIRS
  "."
  PRIV_REQUIRES
  arduino-esp32
  vfs
  fatfs
  controller
  driver)

fatfs_create_spiflash_image(storage storage FLASH_IN_PROJECT)
