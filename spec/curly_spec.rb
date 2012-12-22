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

  it "runs the specs" do
    resp = Curly::Request.get("http://localhost:4567")

    resp.status.should eq(200)
    resp.body.should   eq("hello world")
  end

end
