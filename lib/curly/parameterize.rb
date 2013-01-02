require 'cgi'

# This class converts a hash of http request parameters into a query string of
# the form key=value&key2=value2&key3=value3... in a format compatible to the
# +Object.to_query+ method provided by ActiveSupport.  This class will only be
# used if Curly detects that ActiveSupport's +to_query+ is not present.
module Curly::Parameterize
  class << self

    # Convert the given +params+ hash into a URLencoded query string.
    def query_string(params)
      params.collect { |key, value| to_param(key, value) }.sort * '&'
    end

    private

    def to_param(key, value)
      if value.instance_of?(Array)
        value.collect{ |v| to_param("#{key}[]", v) }.join('&')
      elsif value.instance_of?(Hash)
        value.collect{ |k, v| to_param("#{key}[#{k}]", v) }.sort * '&'
      else
        "#{CGI.escape(key.to_s)}=#{CGI.escape(value.to_s)}"
      end
    end
  end
end
