#include <joint_dispatcher/Dispatcher.hpp>

using namespace joint_dispatcher;

Dispatcher::ChannelID Dispatcher::addInput(std::string const& name)
{
    mInputs.push_back(Input(name));
    return mInputs.size() - 1;
}

Dispatcher::ChannelID Dispatcher::addOutput(std::string const& name, size_t size)
{
    mOutputs.push_back(Output(name));
    mOutputs.back().resize(size);
    return mInputs.size() - 1;
}

Dispatcher::ChannelID Dispatcher::addOutput(std::string const& name, std::vector<std::string> const& jointNames)
{
    mOutputs.push_back(Output(name));
    mOutputs.back().resize(jointNames);
    return mInputs.size() - 1;
}

Dispatcher::ChannelID Dispatcher::getInputByName(std::string const& name) const
{
    for (size_t i = 0; i < mInputs.size(); ++i)
        if (mInputs[i].getName() == name)
            return i;
    throw std::runtime_error("there is no declared input named " + name);
}

Dispatcher::ChannelID Dispatcher::getOutputByName(std::string const& name) const
{
    for (size_t i = 0; i < mOutputs.size(); ++i)
        if (mOutputs[i].getName() == name)
            return i;
    throw std::runtime_error("there is no declared output named " + name);
}

void Dispatcher::addDispatch(
        ChannelID input,  JointSelection const& inputJoints,
        ChannelID output, JointSelection const& outputJoints)
{
    SingleDispatch dispatch;
    dispatch.input = inputJoints;
    dispatch.output_channel = &mOutputs[output];
    dispatch.output = outputJoints;
    // The names should be resolv-able right now
    dispatch.output.resolveNames(mOutputs[output].read());
    mInputs[input].dispatches.push_back(dispatch);
}

void Dispatcher::addDispatch(
        std::string const& input, JointSelection const& inputJoints,
        std::string const& output, JointSelection const& outputJoints)
{
    return addDispatch(getInputByName(input), inputJoints,
            getOutputByName(output), outputJoints);
}

void Dispatcher::write(ChannelID input, base::samples::Joints const& sample)
{
    mInputs[input].write(sample);
}

void Dispatcher::write(std::string const& input, base::samples::Joints const& sample)
{
    write(getInputByName(input), sample);
}

bool Dispatcher::read(ChannelID output, base::samples::Joints& sample)
{
    if (mOutputs[output].isNew())
    {
        sample = mOutputs[output].read();
        return true;
    }
    return false;
}

bool Dispatcher::read(std::string const& name, base::samples::Joints& sample)
{
    return read(getOutputByName(name), sample);
}
