set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

idf_component_register(
  SRCS
  "main.cxx"
  "SerialCommunicator.cxx"
  "BLEDataServer.cxx"
  INCLUDE_DIRS
  "."
  PRIV_REQUIRES
  bt
  vfs
  fatfs
  controller
  driver)

fatfs_create_spiflash_image(storage storage FLASH_IN_PROJECT)
