module Curly
  class Response
    attr_reader :code
    attr_reader :headers
    attr_reader :body
    attr_reader :curl_code

    def initialize
      @code    = 0
      @headers = {}
      @body    = ''
    end


    private

    def add_header(line)
      line = line.chomp.split(': ', 2)
      return unless line.length == 2

      @headers[line[0]] = line[1]
    end

    def add_body(data)
      @body << data;
    end

  end
end
