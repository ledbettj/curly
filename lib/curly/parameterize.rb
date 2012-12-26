require 'cgi'

module Curly::Parameterize
  class << self

    def query_string(params, namespace = nil)
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
