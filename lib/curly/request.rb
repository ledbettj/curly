require 'uri'

class Curly::Request
  class << self

    def get(url, opts = {})
      opts[:method] = :get
      Curly::Request.new(url, opts).run
    end

    def post(url, opts = {})
      opts[:method] = :post
      Curly::Request.new(url, opts).run
    end

    def put(url, opts = {})
      opts[:method] = :put
      Curly::Request.new(url, opts).run
    end

    def delete(url, opts = {})
      opts[:method] = :delete
      Curly::Request.new(url, opts).run
    end

  end

  attr_accessor :url
  attr_accessor :body, :follow_location, :headers, :method, :params
  attr_accessor :ssl_verify_peer, :timeout

  attr_reader :response

  def initialize(url, opts = {})
    @url              = url
    @body             = opts[:body]
    @follow_location  = opts[:follow_location] == false ? false : true
    @headers          = opts[:headers]
    @method           = opts[:method]
    @params           = opts[:params]
    @timeout          = opts[:timeout]
    @ssl_verify_peer  = opts[:ssl_verify_peer] == false ? false : true
  end

  def effective_url
    return @url unless @params

    uri = URI.parse(@url)
    sep = uri.query ? "&" : "?"

    "#{@url}#{sep}#{Curly::Parameterize.query_string(@params)}"
  end
end
