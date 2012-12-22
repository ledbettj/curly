class Curly::Request
  class << self

    def get(url, opts = nil)
      Curly::Request.new.get(url, opts)
    end

    def post(url, opts = nil)
      Curly::Request.post(url, opts)
    end

  end
end
