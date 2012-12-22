module Curly
  class Response
    attr_reader :code
    attr_reader :headers
    attr_reader :body
    attr_reader :curl_code
    attr_reader :curl_error

    def initialize
      @code    = 0
      @headers = {}
      @body    = ''
    end

  end
end
