require 'spec_helper'
require 'spec_server'
require 'curly'
require 'pry'

describe "Curly" do
  before(:all) do
    SpecServer.start
  end

  after(:all) do
    SpecServer.stop
  end

  it "returns the appropriate status code" do
    resp = Curly::Request.get("http://localhost:4567/status-test")

    resp.status.should eq(418)
  end

  it "returns the correct body" do
    resp = Curly::Request.get("http://localhost:4567/body-test")

    resp.status.should eq(200)
    JSON.parse(resp.body).should eq({
        'value' => 1234
      })
  end

end
