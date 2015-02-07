import java.awt.Frame;
import java.awt.Image;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.InputStream;
import java.util.Arrays;

import javax.imageio.ImageIO;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.ScrollPaneConstants;
import javax.swing.UIManager;

import com.google.api.translate.Language;
import com.google.api.translate.Translate;

public class Sim2TradFrame extends JFrame {
	private static final long serialVersionUID = 1L;

	static String sim2Trad(String sim) throws Exception {
		String[] sims = sim.split("\n");
		StringBuilder sb = new StringBuilder();
		for (int i = 0; i < sims.length; i ++) {
			if (sims[i].length() > 0) {
				sb.append(Translate.translate(sims[i],
						Language.CHINESE_SIMPLIFIED, Language.CHINESE_TRADITIONAL));
			}
			sb.append('\n');
		}
		return sb.toString();
	}

	static String gb2Big5(String gb) throws Exception {
		return new String(gb.getBytes("BIG5"));
	}

	public Sim2TradFrame() {
		super("简繁转换");
		setLayout(null);
		setLocationByPlatform(true);
		setResizable(false);

		Insets insets = new Insets(0, 0, 0, 0);
		KeyAdapter ka = new KeyAdapter() {
			@Override
			public void keyPressed(KeyEvent e) {
				if (e.getKeyCode() == KeyEvent.VK_ESCAPE) {
					dispose();
				}
			}
		};

		final JTextArea txtLeft = new JTextArea();
		// txtLeft.enableInputMethods(false);
		txtLeft.setFont(getFont());
		txtLeft.addKeyListener(ka);
		JScrollPane spLeft = new JScrollPane(txtLeft,
				ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
				ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
		spLeft.setBounds(5, 5, 225, 100);
		add(spLeft);

		final JTextArea txtRight = new JTextArea();
		txtRight.setFont(getFont());
		txtRight.setEditable(false);
		txtRight.addKeyListener(ka);
		JScrollPane spRight = new JScrollPane(txtRight,
				ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
				ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
		spRight.setBounds(5, 110, 225, 100);
		add(spRight);

		JButton btnTrad = new JButton("繁体");
		btnTrad.setBounds(235, 5, 80, 30);
		btnTrad.setMargin(insets);
		btnTrad.addKeyListener(ka);
		btnTrad.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				try {
					txtRight.setText(sim2Trad(txtLeft.getText()));
				} catch (Exception ex) {
					JOptionPane.showMessageDialog(Sim2TradFrame.this, ex,
							getTitle(), JOptionPane.WARNING_MESSAGE);
				}
			}
		});
		add(btnTrad);

		JButton btnBig5 = new JButton("BIG5");
		btnBig5.setBounds(235, 45, 80, 30);
		btnBig5.setMargin(insets);
		btnBig5.addKeyListener(ka);
		btnBig5.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				try {
					txtRight.setText(gb2Big5(txtLeft.getText()));
				} catch (Exception ex) {
					JOptionPane.showMessageDialog(Sim2TradFrame.this, ex,
							getTitle(), JOptionPane.WARNING_MESSAGE);
				}
			}
		});
		add(btnBig5);

		JButton btnTradBig5 = new JButton("繁体BIG5");
		btnTradBig5.setBounds(235, 85, 80, 30);
		btnTradBig5.setMargin(insets);
		btnTradBig5.addKeyListener(ka);
		btnTradBig5.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				try {
					txtRight.setText(gb2Big5(sim2Trad(txtLeft.getText())));
				} catch (Exception ex) {
					JOptionPane.showMessageDialog(Sim2TradFrame.this, ex,
							getTitle(), JOptionPane.WARNING_MESSAGE);
				}
			}
		});
		add(btnTradBig5);

		JButton btnExit = new JButton("退出");
		btnExit.setBounds(235, 180, 80, 30);
		btnExit.setMargin(insets);
		btnExit.addKeyListener(ka);
		btnExit.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				dispose();
			}
		});
		add(btnExit);

		addWindowListener(new WindowAdapter() {
			@Override
			public void windowOpened(WindowEvent e) {
				Insets i = getInsets();
				setSize(320 + i.left + i.right, 215 + i.top + i.bottom);
			}

			@Override
			public void windowClosing(WindowEvent e) {
				dispose();
			}

			@Override
			public void windowClosed(WindowEvent e) {
				// Clear all non-focusable windows including MethodInputJFrame
				for (Frame frame : getFrames()) {
					if (!frame.isFocusableWindow()) {
						frame.dispose();
					}
				}
			}
		});

		Class<Sim2TradFrame> clazz = Sim2TradFrame.class;
		InputStream in16 = clazz.getResourceAsStream("Sim2TradIcon16.gif");
		InputStream in32 = clazz.getResourceAsStream("Sim2TradIcon32.gif");
		InputStream in48 = clazz.getResourceAsStream("Sim2TradIcon48.gif");
		try {
			setIconImages(Arrays.asList(new Image[] {ImageIO.read(in16),
					ImageIO.read(in32), ImageIO.read(in48)}));
			in16.close();
			in32.close();
			in48.close();
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
	}

	public static void main(String[] args) throws Exception {
		Translate.setHttpReferrer("http://code.google.com/p/google-api-translate-java/");
		UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		Sim2TradFrame frame = new Sim2TradFrame();
		frame.setVisible(true);
	}
}