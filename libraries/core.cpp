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
  stream->writeElement(entering ? ">>" : "<<");
  stream->writeElement(scopeName);
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
void buildExceptionMessagePart (const std::exception &exception, std::string &r_out) {
  const char *subMsg = exception.what();
  bool headIsCapitalisable = true;
  if (*subMsg == '_') {
    headIsCapitalisable = false;
    ++subMsg;
  }

  if (r_out.empty()) {
    r_out.append(subMsg);
    if (headIsCapitalisable && !r_out.empty()) {
      char &c = r_out.front();
      if (c == static_cast<unsigned char>(c)) {
        c = static_cast<char>(std::toupper(c));
      }
    }
  } else {
    r_out.append(": ");
    r_out.append(subMsg);
  }

  try {
    std::rethrow_if_nested(exception);
  } catch (const std::exception &e) {
    buildExceptionMessagePart(e, r_out);
    return;
  } catch (...) {
    // Recurse no further.
  }
  r_out.append(".");
  return;
}

std::string buildExceptionMessage (const std::exception &rootException) {
  std::string out;

  try {
    buildExceptionMessagePart(rootException, out);
  } catch (...) {
    out = "An error occurred (but further detail is unavailable, as an error occurred while building the message).";
  }

  return out;
}

PlainException::PlainException (const std::string &msg) : literalMsg(nullptr), composedMsg(new std::string(msg)) {
}

PlainException::PlainException (std::string &&msg) : literalMsg(nullptr), composedMsg(new std::string(move(msg))) {
}

PlainException::PlainException (const char *msg) noexcept : literalMsg(msg), composedMsg(nullptr) {
}

PlainException PlainException::create (const char *msgTemplate) {
  const char *i = msgTemplate;
  char c;
  while ((c = *(i++)) != 0) {
    if (c == '%') {
      --i;

      std::string msg(msgTemplate, i);
      interpolate(i, msg);
      return PlainException(move(msg));
    }
  }

  return PlainException(msgTemplate);
}

void PlainException::interpolate (const char *msgTemplate, std::string &r_out) {
  char c;
  while ((c = *(msgTemplate++)) != 0) {
    if (c == '%') {
      c = *msgTemplate;
      if (c != 0) {
        ++msgTemplate;
        switch (c) {
          case '%':
            r_out.push_back('%');
            break;
          default:
            break;
        }
      }
    } else {
      r_out.push_back(c);
    }
  }
}

const char *PlainException::what () const noexcept {
  return literalMsg ? literalMsg : composedMsg->c_str();
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
}
