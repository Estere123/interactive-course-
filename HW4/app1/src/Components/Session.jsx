import { Session, Chatbox } from "@talkjs/react";

export default function Chat() {
  return (
    <Session appId="t78P9qHj" userId="sample_user_alice">
      <Chatbox conversationId="sample_conversation" />
    </Session>
  );
}