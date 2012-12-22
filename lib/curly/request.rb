class Curly::Request
  class << self

    def get(url)
      c = Curly::Request.new
      c.get(url)
    end

    def post(url)
      c = Curly::Request.new
      c.post(url)
    end

  end
end
