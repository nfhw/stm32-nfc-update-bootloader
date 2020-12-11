# NFC Intercom

## Command Message Device → App

### Common Message

NFC Comms are conceived from FTM Demo protocol:

- FTM *(Fast Transfer Mode)* Protocol is inherited from STMicroelectronics described as Figure 18,19 at <https://www.st.com/resource/en/user_manual/dm00288894.pdf#page=21>.
  - **Note:** *FTM Protocol* implies protocol conceived by ST25 manual for demo purposes, while *FTM* is a mode of operation for transferring data between I2C and NFC buses.

### Common Frame

The tags in square braces have following meanings:

- `BL`: Command is understood by bootloader firmware.
- `FW`: Command is understood by main firmware.
- exactly one of:
  - `D2H`: Communication initiated by gadget device, e.g. LoRa button.
  - `H2D`: Communication initiated by external host, e.g. phone device.

<table>
  <tr>
    <th>Byte</th>
    <th>Content</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>0</td>
    <td>Fct</td>
    <td>
      Function<br>
    </td>
  </tr>
  <tr>
    <td>1</td>
    <td>C/R/A</td>
    <td>
      0: Command<br>
      1: Response<br>
      2: Acknowledge<br>
    </td>
  </tr>
  <tr>
    <td>2</td>
    <td>Err</td>
    <td>
      0: No Error<br>
      1: Default Error<br>
      2: Unknown Function<br>
      3: Bad Request<br>
      4: Length Error<br>
      5: Chunk Error<br>
      6: Protocol Error<br>
    </td>
  </tr>
  <tr>
    <td>3</td>
    <td>Chain</td>
    <td>
      0: Simple Frame<br>
      1: Chained Frame<br>
    </td>
  </tr>
</table>

### Simple Frame

<table>
  <tr>
    <th>Extends</th>
    <td>Common Frame</td>
  </tr>
</table>

<table>
  <tr>
    <th>Byte</th>
    <th>Content</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>0</td>
    <td>Fct</td>
    <td>
      <code>0x08: [H2D BL FW]</code> Present Password<br>
    </td>
  </tr>
  <tr>
    <td>3</td>
    <td>Chain</td>
    <td>
      0: Simple Frame<br>
    </td>
  </tr>
  <tr>
    <td>4</td>
    <td>Len</td>
    <td>
      0 .. 251: Length of data<br>
    </td>
  </tr>
  <tr>
    <td>5 .. 255</td>
    <td>Data</td>
    <td>
      Content determined by <em>Fct</em> and <em>C/R/A.</em><br>
    </td>
  </tr>
</table>

### Chained Frame

**Note:** FTM protocol header employs big endian values, where low offset is most significant byte. e.g. `0x00 0x01` is value 1.

<table>
  <tr>
    <th>Extends</th>
    <td>Common Frame</td>
  </tr>
</table>

<table>
  <tr>
    <th>Byte</th>
    <th>Content</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>0</td>
    <td>Fct</td>
    <td>
      <code>0x04: [H2D BL FW]</code> Upload Main Firmware<br>
    </td>
  </tr>
  <tr>
    <td>3</td>
    <td>Chain</td>
    <td>
      1: Chained Frame<br>
    </td>
  </tr>
  <tr>
    <td>4 .. 7</td>
    <td>Full Len</td>
    <td>
      0 .. 4294967295: Length of all data (all chunks) in chained frame.<br>
    </td>
  </tr>
  <tr>
    <td>8 .. 9</td>
    <td>Chunk Cnt</td>
    <td>
      0 .. 65535: Total number of chunks in chained frame.<br>
    </td>
  </tr>
  <tr>
    <td>10 .. 11</td>
    <td>Chunk Nr</td>
    <td>
      0 .. 65535: Current chunk number in chained frame.<br>
    </td>
  </tr>
  <tr>
    <td>12</td>
    <td>Len</td>
    <td>
      0 .. 243: Current chunk number in chained frame.<br>
    </td>
  </tr>
  <tr>
    <td>13 .. 255</td>
    <td>Data</td>
    <td>
      Content determined by <em>Fct</em> and <em>C/R/A</em>.<br>
      Offset from <em>Chunk Nr</em> * 243.<br>
    </td>
  </tr>
</table>

## Password Messages

Password is sent as discrete message. On valid entry, all consecutive messages
are privileged for 120 seconds. Invalid or empty password clears timeout,
and in case of bootloader, reboots to bootloader if password was valid.

**Note:** Password stored in *Data* can be any byte value `0x00 .. 0xff`,
and is exactly 4 bytes. Further, consecutive messages prolong timeout to 120
seconds, this accommodates firmware upload that may take 5 minutes.

<table>
  <tr>
    <th>Extends</th>
    <td>Simple Frame</td>
  </tr>
  <tr>
    <th>Privileged</th>
    <td>No</td>
  </tr>
</table>

<table>
  <tr>
    <th rowspan="2">Step</th>
    <th rowspan="2">Sender</th>
    <th colspan="6">Wire format</th>
    <th rowspan="2">Description</th>
  </tr>
  <tr>
    <th>Fct</th>
    <th>C/R/A</th>
    <th>Err</th>
    <th>Chain</th>
    <th>Len</th>
    <th>Data</th>
  </tr>
  <tr>
    <td>A</td>
    <td>Host</td>
    <td><code>0x08</code></td>
    <td><code>0x01</code></td>
    <td><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td><code>0x04</code></td>
    <td><code>0x12&nbsp;0x34&nbsp;0x56&nbsp;0x78</code></td>
    <td>Present a password 12345678.</td>
  </tr>
  <tr>
    <td>B</td>
    <td>Device</td>
    <td><code>0x08</code></td>
    <td><code>0x01</code></td>
    <td><code>0x03</code></td>
    <td><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td>N/A</td>
    <td>Tell password is invalid. Timeout cleared.</td>
  </tr>
  <tr>
    <td>B</td>
    <td>Device</td>
    <td><code>0x08</code></td>
    <td><code>0x01</code></td>
    <td><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td>N/A</td>
    <td>Tell password is valid. Timeout set 120 seconds.</td>
  </tr>
</table>

## Bootloader Messages

Firmware is sent as a series of messages, prepended by password authentication.
The upload commands work from either bootldr or mainfw transparently. During
upload, transaction is unidirectional, every message is guaranteed by st25dv
noticing when phone has read its mailbox, blocking further writes otherwise.
Maximum firmware upload size is 160KiB (163840B). Transfer speed is 178B/s, thus
67 KiB binary may take about 5 minutes. Some modern phone can do 1.5 minutes.

Once last chunk is received, device produces crc32 checksum of firmware in
flash, and replies that to ST25DV app. ST25DV acknowledges the checksum with
final message. If good, boots mainfw, otherwise device sits in bootloader
indefinitely.

In case the mainfw has been borked, there's a 4 second delay before bootldr
boots mainfw during power-up, given valid password, the timeout extends to
120 seconds, with opportunity to re-flash.

**Note:** Firmware is written eagerly, don't cut the streams.

<table>
  <tr>
    <th>Extends</th>
    <td>Chained Frame</td>
  </tr>
  <tr>
    <th>Privileged</th>
    <td>Required</td>
  </tr>
</table>

<table>
  <tr>
    <th rowspan="2">Step</th>
    <th rowspan="2">Sender</th>
    <th colspan="9">Wire format</th>
    <th rowspan="2">Description</th>
  </tr>
  <tr>
    <th>Fct</th>
    <th>C/R/A</th>
    <th>Err</th>
    <th>Chain</th>
    <th>Full Len</th>
    <th>Chunk Cnt</th>
    <th>Chunk Nr</th>
    <th>Len</th>
    <th>Data</th>
  </tr>
  <tr>
    <td>C</td>
    <td>Host</td>
    <td><code>0x04</code></td>
    <td><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td><code>0x01</code></td>
    <td><code>0x00&nbsp;0x01&nbsp;0x0d&nbsp;0x30</code></td>
    <td><code>0x01&nbsp;0x1c</code></td>
    <td><code>0x00&nbsp;0x01</code></td>
    <td><code>0xf3</code></td>
    <td>
      <code>0x00&nbsp;0x50&nbsp;0x00&nbsp;0x20<br></code>
      <code>0x6d&nbsp;0x3c&nbsp;0x01&nbsp;0x08<br></code>
      <code>0xed&nbsp;0x3c&nbsp;0x01&nbsp;0x08<br></code>
      <code>0xef&nbsp;0x3c&nbsp;0x01&nbsp;0x08<br></code>
      <code>...</code>
    </td>
    <td>Start Uploading new mainfw. 1st chunk.</td>
  </tr>
  <tr>
    <td>D</td>
    <td>Host</td>
    <td><code>0x04</code></td>
    <td><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td><code>0x01</code></td>
    <td><code>0x00&nbsp;0x01&nbsp;0x0d&nbsp;0x30</code></td>
    <td><code>0x01&nbsp;0x1c</code></td>
    <td><code>0x00&nbsp;0x02</code></td>
    <td><code>0xf3</code></td>
    <td>
      <code>...</code>
    </td>
    <td>2nd chunk.</td>
  </tr>
  <tr>
    <td>E..JY</td>
    <td colspan="11">...</td>
  </tr>
  <tr>
    <td>JZ</td>
    <td>Host</td>
    <td><code>0x04</code></td>
    <td><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td><code>0x01</code></td>
    <td><code>0x78&nbsp;0x01&nbsp;0x0d&nbsp;0x30</code></td>
    <td><code>0x01&nbsp;0x1c</code></td>
    <td><code>0x01&nbsp;0x1c</code></td>
    <td><code>0x8f</code></td>
    <td>
      <code>...</code>
    </td>
    <td>Final chunk.</td>
  </tr>
  <tr>
    <td>KA</td>
    <td>Device</td>
    <td><code>0x04</code></td>
    <td><code>0x01</code></td>
    <td><code>0x00</code></td>
    <td colspan="4"><code>0x00</code></td>
    <td><code>0x04</code></td>
    <td>
      <code>0x78&nbsp;0x56&nbsp;0x34&nbsp;0x12</code>
    </td>
    <td>Respond crc32 of flashed firmware.</td>
  </tr>
  <tr>
    <td>KB</td>
    <td>Host</td>
    <td><code>0x04</code></td>
    <td><code>0x02</code></td>
    <td><code>0x00</code></td>
    <td colspan="4"><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td>N/A</td>
    <td>Acknowledge the crc32 is good.</td>
  </tr>
  <tr>
    <td>KB</td>
    <td>Host</td>
    <td><code>0x04</code></td>
    <td><code>0x02</code></td>
    <td><code>0x01</code></td>
    <td colspan="4"><code>0x00</code></td>
    <td><code>0x00</code></td>
    <td>N/A</td>
    <td>Acknowledge the crc32 is bad.</td>
  </tr>
</table>
