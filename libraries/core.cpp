#include "core.hpp"
#include "core.using"

namespace core {

void checkVersionImpl (const char *requirer, const char *requiree, Version required, Version available) noexcept {
  if (required.major != available.major || required.minor > available.minor) {
    fprintf(stderr, "%s depends on %s %u.%u or compatible, but %u.%u is present\n", requirer, requiree, required.major, required.minor, available.major, available.minor);
    fflush(stderr);
    dieHard();
  }
}

const Version VERSION{LIB_MAJ, LIB_MIN}; DEPENDENCIES;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
#ifndef NDEBUG
namespace debug {

const char *const Stream::INDENT = "  ";

Stream::Stream () noexcept : handle(stderr), indentCount(0), atStartOfLine(true) {
}

Stream::Stream (const char *filename) noexcept : indentCount(0), atStartOfLine(true) {
  if (!filename) dieHard();

  handle = fopen(filename, "wb");
  if (!handle) {
    fprintf(stderr, "failed to open Stream to %s\n", filename);
    fflush(stderr);
    dieHard();
  }
}

Stream::~Stream () noexcept {
  if (handle != stderr) {
    fclose(handle);
  }
}

void Stream::enterScope () noexcept {
  ++indentCount;
}

void Stream::exitScope () noexcept {
  if (indentCount == 0) dieHard();
  --indentCount;
}

void Stream::startLine () noexcept {
  if (!atStartOfLine) {
    return;
  }

  for (iu i = 0; i < indentCount; ++i) {
    writeElement(INDENT);
  }
  atStartOfLine = false;
}

void Stream::endLine () noexcept {
  writeElement("\n");
  atStartOfLine = true;
}

void Stream::flush () noexcept {
  fflush(handle);
}

void Stream::writeElement (const char *value) noexcept {
  int r = fputs(value, handle);
  if (r < 0) dieHard("failed to write to stream\n");
}

Logger::ScopeProxy::ScopeProxy (Logger &logger, const char *scopeName) noexcept
  : stream(logger.stream), scopeName(scopeName)
{
  if (!scopeName) dieHard();

  if (stream) {
    writeMarker(true);
    stream->enterScope();
  }
}

Logger::ScopeProxy::~ScopeProxy () noexcept {
  if (stream) {
    stream->exitScope();
    writeMarker(false);
  }
}

void Logger::ScopeProxy::writeMarker (bool entering) noexcept {
  stream->startLine();
  stream->writeElement(entering ? "Entering " : "Exiting ");
  stream->writeElement(scopeName);
  stream->writeElement(":");
  stream->endLine();
  stream->flush();
}

Logger::Logger () noexcept : stream(nullptr) {
}

void Logger::open (const std::shared_ptr<Stream> &stream) noexcept {
  this->stream = stream;
}

void Logger::close () noexcept {
  this->stream.reset();
}

Logger::ScopeProxy Logger::defineScope (const char *scopeName) noexcept {
  return ScopeProxy(*this, scopeName);
}

void Logger::writeElements () noexcept {
  // That's all, folks.
}

}
#endif

DC();

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
noreturn void dieHard () noexcept {
  volatile int i = 0;
  int x = 0 / (i - i);

  exit(EXIT_FAILURE + static_cast<int>(x - x));
}

noreturn void dieHard (const char *msg) noexcept {
  fputs(msg, stderr);
  fflush(stderr);
  dieHard();
}

void check (const bool &cond) noexcept {
  if (!cond) {
    dieHard("test failed\n");
  }
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
GeneralException::GeneralException (const std::string &msg) : msg(msg) {
}

GeneralException::GeneralException (std::string &&msg) : msg(move(msg)) {
}

GeneralException::GeneralException (const char *msg) : msg(msg) {
}

const char *GeneralException::what() const noexcept {
  return msg.c_str();
}

}
