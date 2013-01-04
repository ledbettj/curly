class Curly::Response
  attr_reader :body, :head, :status, :curl_code, :curl_error

  alias_method :code, :status

  alias_method :curl_return_code,   :curl_code
  alias_method :curl_error_message, :curl_error

  def initialize
    @body   = ''
    @head   = ''
    @status = 0
  end

  # returns +true+ if the http status code returned by the server falls within
  # the 2xx range, +false+ otherwise.
  def success?
    @status >= 200 && @status < 300
  end

  def headers
    @headers ||= @head.split(/\r\n/).each_with_object({}) do |line, h|
      key, value = line.split(': ', 2)
      next unless value

      # There may be multiple headers with the same key and different values.
      # for example, Set-Cookie.
      if !h.has_key?(key)
        h[key] = value
      elsif h[key].instance_of?(Array)
        h[key].push(value)
      else
        h[key] = [h[key], value]
      end
    end
  end

end
