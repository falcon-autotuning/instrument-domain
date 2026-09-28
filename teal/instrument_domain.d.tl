global record DomainSpec
   min: number 
   max: number 
end

global record Domain 
   get_min: function(self: Domain): number 
   get_max: function(self: Domain): number 
   clone: function(self: Domain): Domain
   to_string: function(self: Domain): string
end

global record instrument_domain
   new: function(DomainSpec): Domain
end
