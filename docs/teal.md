# Using instrument-domain with Teal

The Lua binding exposes `InstrumentDomain` objects as userdata with methods.  
These can be used in Teal via a type declaration file.

See the `teal/instrument-domain.d.tl` for explicit types.

## Example

```lua
local domain: InstrumentDomain= domain-- injected from C

local min: number= domain:get_min()
local max: number= domain:get_max()
