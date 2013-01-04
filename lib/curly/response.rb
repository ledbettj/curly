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

  def success?
    @status >= 200 && @status < 300
  end

  def headers
    @headers ||= @head.split(/\r\n/).each_with_object({}) do |line, h|
      key, value = line.split(': ', 2)
      h[key] = value if value
    end
  end

end
