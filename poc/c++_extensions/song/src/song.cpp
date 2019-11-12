#include <iostream>
#include <string>

#include "song.h"


Song::Song(const std::string& name, double length) :
    m_name(name), m_length(length)
{}


void Song::SetName(const std::string& name)
{
    this->m_name = name;
}


void Song::SetLength(double length)
{
    this->m_length = length;
}


std::string Song::GetName() const
{
    return this->m_name;
}


double Song::GetLength() const
{
    return this->m_length;
}


void Song::PrintMessage() const
{
    std::cout << "Hello from " << this->m_name << std::endl;
}