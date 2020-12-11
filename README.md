# STM32 Bootloader for NFC Based Firmware Updates

This bootloader is intended for embedded devices that support a firmware update over NFC.
This is often useful for LPWAN devices which need an out-of-band capability to update the firmware.
The currenclty supported hardware is the STM32L0 MCU and the ST25DV NFC chip.

## Features

- Password protected firmware update (Not recoverable! Password read from EEPROM. Only mainfw can change password.)
- CRC verification for fault resistent updates
- On failure, precludes further boot up indefinitely.
- LED User feedback on firmware update process
	- Green LED toggles on every received packet.
	- Red LED blinks on finishing a failed update.
- Upgrading firmware possible in two ways:
	- On every boot up, the device waits for 4 seconds for the user to initiale an update process using a mobile app. Prevents device from bricking in case firmware is broken or update failed.
	- On normal operation, app can block ST25DV mailbox, writes a flag to EEPROM and reboots to bootloader.
- Retain compatibility with "ST25 NFC Tap" phone app.
	- Pasword thus is 8 hexadecumal digits.
	- App to bootloader handoff achieved via blocking ST25DV via partial read.

## User Experience for End Users

Using a mobile app:

- Type in password. Pre-defined password is `12345678`.
- Select the firmware binary of choice.
- Select to start the transfer.
- Tap smartphone onto the device.
- Progress bar is updated and device green LED toggles as the firmware is transferred.
- If firmware CRCs match after the transfer, device reboots itself, otherwise red LED blinks.

## Detailed technical Procedure

- Method 1: mainfw Method
	- Application listens to same NFC commands, reboot to bootldr happens only once firmware update starts, and firmware update allowed only after valid password was provided.
	- Phone is blocked from further writes as MCU performs only a partial read of ST25DV mailbox, enough to understand that firmware update is incomming.
	- Reboot to bootldr happens by passing information via EEPROM at 0x0808_0000:
		- mainfw writes flags BOOTMODE_WAITNFC_MASK:0x01 BOOTMODE_PASSOK_MASK:0x02 BOOTMODE_KEEPNFC_MASK:0x04
		- bootldr reads and resets these flags. Thus timeout is 120 seconds, not asking a password and not re-initalizing ST25DV to retain its mailbox.
- Method 2: bootldr Method
	- On every boot up, the device waits for 4 seconds for the user to enter valid password, upon which timeout is extended to 120 seconds.
	- If within the 120 seconds there's no NFC activity, the bootloader loads the old firmware.
	- If an invalid password is written after having written a valid password, boot mainfw even if borked.
	- The bootloader can handle intermittent connection outages after the FW transfer process has started.
		- If transfer is initiated, 120 second timeout is disabled, and device waits indefinitely.
		- In case there is a size mismatch issue or an issue related to the number of packets sent and received. Then restart phone application and attempt transfer again, no need to power off device.
		- Phone doesn't write until ST25DV has had its contents read by the MCU. (E.g. reading is a side-effect)

## Memory Map of the Bootloader and Firmware

- Flash Base Address(origin):       `0x0800_0000`
- Size alloted to Bootloader:       `28 KiB`
- Memory allocated to Bootloader:   `19_180 B`
- Firmware Start Address:           `0x0800_8000`
- Memory allocated to Firmware:     `~96 KiB`

## Message Structure from the App

See [MESSAGE_FORMAT_NFC.md](./MESSAGE_FORMAT_NFC.md).

## Legal

Contains automatically generated and manually written copyrighted code from the
following legal entities:

- Arm Limited
- SEGGER Microcontroller GmbH
- STMicroelectronics

These copyrights are noted in the head of the respective files. All other code
is subject to the [LGPL](LICENSE).
