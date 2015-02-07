import java.io.*;
 
public class Pipe {
    private static final int LINE_INPUT_MAX_CHAR = 4096;
    private int nReadEnd = 0;
    private InputStream in = null;
    private PrintStream out = null;
    private byte[] ucBuffer = new byte[LINE_INPUT_MAX_CHAR];

    public void open(String szCommand) throws IOException {
    	if (szCommand == null) {
    		in = System.in;
    		out = System.out;
    	} else {
    		String[] sz = {szCommand};
    		ProcessBuilder pb = new ProcessBuilder(sz);
    		pb.redirectErrorStream(true);
    		Process p = pb.start();
    		in = p.getInputStream();
    		out = new PrintStream(p.getOutputStream());
    	}
    }

    public void close() throws IOException {
        nReadEnd = 0;
        in.close();
        out.close();
    }

    public void lineOutput(String sz) {
        out.println(sz);
        out.flush();
    }

    public String lineInput() throws IOException {
        String sz = getBuffer();
        if (sz == null && in.available() > 0) {
            nReadEnd += in.read(ucBuffer, nReadEnd, LINE_INPUT_MAX_CHAR - nReadEnd);
            sz = getBuffer();
            if (sz == null && nReadEnd == LINE_INPUT_MAX_CHAR) {
                sz = new String(ucBuffer, 0, LINE_INPUT_MAX_CHAR - 1);
                ucBuffer[0] = ucBuffer[LINE_INPUT_MAX_CHAR - 1];
                nReadEnd = 1;
            }
        }
        return sz;
    }

    private String getBuffer() {
        String sz = null;
        int nFeedEnd;
        for (nFeedEnd = 0; nFeedEnd < nReadEnd; nFeedEnd ++) {
            if (ucBuffer[nFeedEnd] == '\n') {
                break;
            }
        }
        if (nFeedEnd < nReadEnd) {
            sz = new String(ucBuffer, 0, nFeedEnd);
            int nStrChr = sz.indexOf('\r');
            if (nStrChr >= 0) {
                 sz = sz.substring(0, nStrChr);
            }
            nFeedEnd ++;
            nReadEnd -= nFeedEnd;
            System.arraycopy(ucBuffer, nFeedEnd, ucBuffer, 0, nReadEnd);
        }
        return sz;
    }
}
