class Curly::Client
  class << self

    def get(url)
      c = Curly::Client.new
      c.get(url)
    end

    def post(url)
      c = Curly::Client.new
      c.post(url)
    end

  end
end
